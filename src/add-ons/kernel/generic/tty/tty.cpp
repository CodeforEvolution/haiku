/*
 * Copyright 2007-2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Copyright 2004-2009, Axel Dörfler, axeld@pinc-software.de.
 * Distributed under the terms of the MIT License.
 */


#include "tty_private.h"

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <util/AutoLock.h>
#include <util/kernel_cpp.h>

#include <team.h>

#include <tty.h>


//#define TTY_TRACE
#ifdef TTY_TRACE
#	define TRACE(x) dprintf x
#else
#	define TRACE(x) ;
#endif


/*
	Locking
	-------

	There are three locks involved. If more than one needs to be held at a
	time, they must be acquired in the order they are listed here.

	gTTYCookieLock: Guards the access to the fields
	tty_cookie::{thread_count,closed}, or more precisely makes access to them
	atomic. thread_count is the number of threads currently using the cookie
	(i.e. read(), write(), ioctl() operations in progress). Together with
	blocking_semaphore this serves the purpose to make sure that all pending
	operations are done at a certain point when closing a cookie
	(cf. tty_close_cookie() and TTYReference).

	tty::lock: Guards the access to tty::{input_buffer,settings::{termios,
	window_size,pgrp_id}}. Moreover when held guarantees that tty::open_count
	won't drop to zero. A tty and the tty connected to it (master and slave)
	share the same lock.

	gTTYRequestLock: Guards access to tty::{reader,writer}_queue (most
	RequestQueue methods do the locking themselves (the lock is a
	recursive_lock)), queued Requests and associated RequestOwners.


	Reading/Writing
	---------------

	Most of the dirty work when dealing with reading/writing is done by the
	{Reader,Writer}Locker classes. Upon construction they lock the tty,
	(tty::lock) create a RequestOwner and queue Requests in the respective
	reader/writer queues (tty::{reader,writer}_queue). The
	Acquire{Reader,Writer}() methods need to be called before being allowed to
	read/write. They ensure that there is actually something to read/space for
	writing -- in blocking mode they wait, if necessary. When destroyed the
	{Reader,Writer}Locker() remove the formerly enqueued Requests and notify
	waiting reader/writer and/or send out select events, whatever is appropiate.

	Acquire{Reader,Writer}() never return without an actual event being
	occurred. Either an error has occurred (return value) -- in this case the
	caller should terminate -- or bytes are available for reading/space for
	writing (cf. AvailableBytes()).
*/


static void tty_notify_select_event(struct tty* tty, uint8 event);
static void tty_notify_if_available(struct tty* tty, struct tty* otherTTY,
	bool notifySelect);


class AbstractLocker {
public:
	AbstractLocker(tty_cookie* cookie)
		:
		fCookie(cookie),
		fBytes(0)
	{
	}

	size_t AvailableBytes() const
		{ return fBytes; }

protected:
	void Lock()
		{ recursive_lock_lock(fCookie->tty->lock); }
	void Unlock()
		{ recursive_lock_unlock(fCookie->tty->lock); }

	tty_cookie*	fCookie;
	size_t		fBytes;
};


class WriterLocker : public AbstractLocker {
public:
								WriterLocker(tty_cookie* sourceCookie);
								~WriterLocker();

			status_t			AcquireWriter(bool dontBlock,
									size_t bytesNeeded);

private:
			size_t				_CheckAvailableBytes() const;
			status_t			_CheckBackgroundWrite() const;

			struct tty*			fSource;
			struct tty*			fTarget;
			RequestOwner		fRequestOwner;
			bool				fEcho;
};


class ReaderLocker : public AbstractLocker {
public:
								ReaderLocker(tty_cookie* cookie);
								~ReaderLocker();

			status_t			AcquireReader(bigtime_t timeout,
									size_t bytesNeeded);

private:
			size_t				_CheckAvailableBytes() const;
			status_t			_CheckBackgroundRead() const;

			struct tty*			fTTY;
			RequestOwner		fRequestOwner;
};


class TTYReferenceLocking {
public:
	inline bool Lock(tty_cookie* cookie)
	{
		MutexLocker _(gTTYCookieLock);

		if (cookie->closed)
			return false;

		cookie->thread_count++;

		return true;
	}

	inline void Unlock(tty_cookie* cookie)
	{
		MutexLocker locker(gTTYCookieLock);

		sem_id semaphore = -1;
		if (--cookie->thread_count == 0 && cookie->closed)
			semaphore = cookie->blocking_semaphore;

		locker.Unlock();

		if (semaphore >= 0) {
			TRACE(("TTYReference: cookie %p closed, last operation done, "
				"releasing blocking sem %" B_PRId32 "\n", cookie, semaphore));

			release_sem(semaphore);
		}
	}
};

typedef AutoLocker<tty_cookie, TTYReferenceLocking> TTYReference;


// #pragma mark -


Request::Request()
	:
	fOwner(NULL),
	fCookie(NULL),
	fBytesNeeded(0),
	fNotified(false),
	fError(false)
{
}


void
Request::Init(RequestOwner* owner, tty_cookie* cookie, size_t bytesNeeded)
{
	fOwner = owner;
	fCookie = cookie;
	fBytesNeeded = bytesNeeded;
	fNotified = false;
	fError = false;
}


void
Request::Notify(size_t bytesAvailable)
{
	if (!fNotified && bytesAvailable >= fBytesNeeded && fOwner) {
		fOwner->Notify(this);
		fNotified = true;
	}
}


void
Request::NotifyError(status_t error)
{
	if (!fError && fOwner) {
		fOwner->NotifyError(this, error);
		fError = true;
		fNotified = true;
	}
}


void
Request::Dump(const char* prefix)
{
	kprintf("%srequest: %p\n", prefix, this);
	kprintf("%s  owner:        %p\n", prefix, fOwner);
	kprintf("%s  cookie:       %p\n", prefix, fCookie);
	kprintf("%s  bytes needed: %lu\n", prefix, fBytesNeeded);
	kprintf("%s  notified:     %s\n", prefix, fNotified ? "true" : "false");
	kprintf("%s  error:        %s\n", prefix, fError ? "true" : "false");
}


// #pragma mark -


RequestQueue::RequestQueue()
	:
	fRequests()
{
}


void
RequestQueue::Add(Request* request)
{
	if (request) {
		RecursiveLocker _(gTTYRequestLock);

		fRequests.Add(request, true);
	}
}


void
RequestQueue::Remove(Request* request)
{
	if (request) {
		RecursiveLocker _(gTTYRequestLock);

		fRequests.Remove(request);
	}
}


void
RequestQueue::NotifyFirst(size_t bytesAvailable)
{
	RecursiveLocker _(gTTYRequestLock);

	if (Request* first = First())
		first->Notify(bytesAvailable);
}


void
RequestQueue::NotifyError(status_t error)
{
	RecursiveLocker _(gTTYRequestLock);

	for (RequestList::Iterator it = fRequests.GetIterator(); it.HasNext();) {
		Request* request = it.Next();
		request->NotifyError(error);
	}
}


void
RequestQueue::NotifyError(tty_cookie* cookie, status_t error)
{
	RecursiveLocker _(gTTYRequestLock);

	for (RequestList::Iterator it = fRequests.GetIterator(); it.HasNext();) {
		Request* request = it.Next();
		if (request->TTYCookie() == cookie)
			request->NotifyError(error);
	}
}


void
RequestQueue::Dump(const char* prefix)
{
	RequestList::Iterator it = fRequests.GetIterator();
	while (Request* request = it.Next())
		request->Dump(prefix);
}


// #pragma mark -


RequestOwner::RequestOwner()
	:
	fConditionVariable(NULL),
	fCookie(NULL),
	fError(B_OK),
	fBytesNeeded(1)
{
	fRequestQueues[0] = NULL;
	fRequestQueues[1] = NULL;
}


/*!	The caller must already hold the request lock.
*/
void
RequestOwner::Enqueue(tty_cookie* cookie, RequestQueue* queue1,
	RequestQueue* queue2)
{
	TRACE(("%p->RequestOwner::Enqueue(%p, %p, %p)\n", this, cookie, queue1,
		queue2));

	fCookie = cookie;

	fRequestQueues[0] = queue1;
	fRequestQueues[1] = queue2;

	fRequests[0].Init(this, cookie, fBytesNeeded);
	if (queue1)
		queue1->Add(&fRequests[0]);
	else
		fRequests[0].Notify(fBytesNeeded);

	fRequests[1].Init(this, cookie, fBytesNeeded);
	if (queue2)
		queue2->Add(&fRequests[1]);
	else
		fRequests[1].Notify(fBytesNeeded);
}


/*!	The caller must already hold the request lock.
*/
void
RequestOwner::Dequeue()
{
	TRACE(("%p->RequestOwner::Dequeue()\n", this));

	if (fRequestQueues[0])
		fRequestQueues[0]->Remove(&fRequests[0]);
	if (fRequestQueues[1])
		fRequestQueues[1]->Remove(&fRequests[1]);

	fRequestQueues[0] = NULL;
	fRequestQueues[1] = NULL;
}


void
RequestOwner::SetBytesNeeded(size_t bytesNeeded)
{
	if (fRequestQueues[0])
		fRequests[0].Init(this, fCookie, bytesNeeded);

	if (fRequestQueues[1])
		fRequests[1].Init(this, fCookie, bytesNeeded);
}


/*!	The request lock MUST NOT be held!
*/
status_t
RequestOwner::Wait(bool interruptable, bigtime_t timeout)
{
	TRACE(("%p->RequestOwner::Wait(%d)\n", this, interruptable));

	status_t error = B_OK;

	RecursiveLocker locker(gTTYRequestLock);

	// check, if already done
	if (fError == B_OK
		&& (!fRequests[0].WasNotified() || !fRequests[1].WasNotified())) {
		// not yet done

		// publish the condition variable
		ConditionVariable conditionVariable;
		conditionVariable.Init(this, "tty request");
		fConditionVariable = &conditionVariable;

		// add an entry to wait on
		ConditionVariableEntry entry;
		conditionVariable.Add(&entry);

		locker.Unlock();

		// wait
		TRACE(("%p->RequestOwner::Wait(): waiting for condition...\n", this));

		error = entry.Wait(
			(interruptable ? B_CAN_INTERRUPT : 0) | B_RELATIVE_TIMEOUT,
			timeout);

		TRACE(("%p->RequestOwner::Wait(): condition occurred: %" B_PRIx32 "\n",
			this, error));

		// remove the condition variable
		locker.Lock();
		fConditionVariable = NULL;
	}

	// get the result
	if (error == B_OK)
		error = fError;

	return error;
}


bool
RequestOwner::IsFirstInQueues()
{
	RecursiveLocker locker(gTTYRequestLock);

	for (int i = 0; i < 2; i++) {
		if (fRequestQueues[i] && fRequestQueues[i]->First() != &fRequests[i])
			return false;
	}

	return true;
}


void
RequestOwner::Notify(Request* request)
{
	TRACE(("%p->RequestOwner::Notify(%p)\n", this, request));

	if (fError == B_OK && !request->WasNotified()) {
		bool notify = false;

		if (&fRequests[0] == request) {
			notify = fRequests[1].WasNotified();
		} else if (&fRequests[1] == request) {
			notify = fRequests[0].WasNotified();
		} else {
			// spurious call
		}

		if (notify && fConditionVariable)
			fConditionVariable->NotifyOne();
	}
}


void
RequestOwner::NotifyError(Request* request, status_t error)
{
	TRACE(("%p->RequestOwner::NotifyError(%p, %" B_PRIx32 ")\n", this, request,
		error));

	if (fError == B_OK) {
		fError = error;

		if (!fRequests[0].WasNotified() || !fRequests[1].WasNotified()) {
			if (fConditionVariable)
				fConditionVariable->NotifyOne();
		}
	}
}


// #pragma mark -


WriterLocker::WriterLocker(tty_cookie* sourceCookie)
	:
	AbstractLocker(sourceCookie),
	fSource(fCookie->tty),
	fTarget(fCookie->other_tty),
	fRequestOwner(),
	fEcho(false)
{
	Lock();

	// Now that the tty pair is locked, we can check, whether the target is
	// open at all.
	if (fTarget->open_count > 0) {
		// The target tty is open. As soon as we have appended a request to
		// the writer queue of the target, it is guaranteed to remain valid
		// until we have removed the request (and notified the
		// tty_close_cookie() pseudo request).

		// get the echo mode
		fEcho = (fSource->is_master
			&& fSource->settings->termios.c_lflag & ECHO) != 0;

		// enqueue ourselves in the respective request queues
		RecursiveLocker locker(gTTYRequestLock);
		fRequestOwner.Enqueue(fCookie, &fTarget->writer_queue,
			(fEcho ? &fSource->writer_queue : NULL));
	} else {
		// target is not open: we set it to NULL; all further operations on
		// this locker will fail
		fTarget = NULL;
	}
}


WriterLocker::~WriterLocker()
{
	// dequeue from request queues
	RecursiveLocker locker(gTTYRequestLock);
	fRequestOwner.Dequeue();

	// check the tty queues and notify the next in line, and send out select
	// events
	if (fTarget)
		tty_notify_if_available(fTarget, fSource, true);
	if (fEcho)
		tty_notify_if_available(fSource, fTarget, true);

	locker.Unlock();

	Unlock();
}


size_t
WriterLocker::_CheckAvailableBytes() const
{
	size_t writable = line_buffer_writable(fTarget->input_buffer);
	if (fEcho) {
		// we can only write as much as is available on both ends
		size_t locallyWritable = line_buffer_writable(fSource->input_buffer);
		if (locallyWritable < writable)
			writable = locallyWritable;
	}
	return writable;
}


status_t
WriterLocker::AcquireWriter(bool dontBlock, size_t bytesNeeded)
{
	if (!fTarget)
		return B_FILE_ERROR;
	if (fEcho && fCookie->closed)
		return B_FILE_ERROR;

	RecursiveLocker requestLocker(gTTYRequestLock);

	// check, if we're first in queue, and if there is space to write
	if (fRequestOwner.IsFirstInQueues()) {
		fBytes = _CheckAvailableBytes();
		if (fBytes >= bytesNeeded)
			return B_OK;
	}

	// We are not the first in queue or currently there's no space to write:
	// bail out, if we shall not block.
	if (dontBlock)
		return B_WOULD_BLOCK;

	// set the number of bytes we need and notify, just in case we're first in
	// one of the queues (RequestOwner::SetBytesNeeded() resets the notification
	// state)
	fRequestOwner.SetBytesNeeded(bytesNeeded);
	if (fTarget)
		tty_notify_if_available(fTarget, fSource, false);
	if (fEcho)
		tty_notify_if_available(fSource, fTarget, false);

	requestLocker.Unlock();

	// block until something happens
	Unlock();
	status_t status = fRequestOwner.Wait(true);
	Lock();

	// RequestOwner::Wait() returns the error, but to avoid a race condition
	// when closing a tty, we re-get the error with the tty lock being held.
	if (status == B_OK) {
		RecursiveLocker _(gTTYRequestLock);
		status = fRequestOwner.Error();
	}

	if (status == B_OK)
		status = _CheckBackgroundWrite();

	if (status == B_OK)
		fBytes = _CheckAvailableBytes();

	return status;
}


status_t
WriterLocker::_CheckBackgroundWrite() const
{
	// only relevant for the slave end and only when TOSTOP is set
	if (fSource->is_master
		|| (fSource->settings->termios.c_lflag & TOSTOP) == 0) {
		return B_OK;
	}

	pid_t processGroup = getpgid(0);
	if (fSource->settings->pgrp_id != 0
			&& processGroup != fSource->settings->pgrp_id) {
		if (team_get_controlling_tty() == fSource)
			send_signal(-processGroup, SIGTTOU);
	}

	return B_OK;
}


//	#pragma mark -


ReaderLocker::ReaderLocker(tty_cookie* cookie)
	:
	AbstractLocker(cookie),
	fTTY(cookie->tty),
	fRequestOwner()
{
	Lock();

	// enqueue ourselves in the reader request queue
	RecursiveLocker locker(gTTYRequestLock);
	fRequestOwner.Enqueue(fCookie, &fTTY->reader_queue);
}


ReaderLocker::~ReaderLocker()
{
	// dequeue from reader request queue
	RecursiveLocker locker(gTTYRequestLock);
	fRequestOwner.Dequeue();

	// check the tty queues and notify the next in line, and send out select
	// events
	struct tty* otherTTY = fCookie->other_tty;
	tty_notify_if_available(fTTY, (otherTTY->open_count > 0 ? otherTTY : NULL),
		true);

	locker.Unlock();

	Unlock();
}


status_t
ReaderLocker::AcquireReader(bigtime_t timeout, size_t bytesNeeded)
{
	if (fCookie->closed)
		return B_FILE_ERROR;

	status_t status = _CheckBackgroundRead();
	if (status != B_OK)
		return status;

	// check, if we're first in queue, and if there is something to read
	if (fRequestOwner.IsFirstInQueues()) {
		fBytes = _CheckAvailableBytes();
		if (fBytes >= bytesNeeded)
			return B_OK;
	}

	if (fCookie->other_tty->open_count == 0
		&& fCookie->other_tty->opened_count > 0) {
		TRACE(("ReaderLocker::AcquireReader() opened_count %" B_PRId32 "\n",
			fCookie->other_tty->opened_count));
		return B_FILE_ERROR;
	}

	// We are not the first in queue or currently there's nothing to read:
	// bail out, if we shall not block.
	if (timeout <= 0)
		return B_WOULD_BLOCK;

	// reset the number of bytes we need
	fRequestOwner.SetBytesNeeded(bytesNeeded);

	// block until something happens
	Unlock();
	status = fRequestOwner.Wait(true, timeout);
	Lock();

	if (status == B_OK)
		status = _CheckBackgroundRead();

	fBytes = _CheckAvailableBytes();

	TRACE(("ReaderLocker::AcquireReader() ended status 0x%" B_PRIx32 "\n",
		status));

	return status;
}


size_t
ReaderLocker::_CheckAvailableBytes() const
{
	// Reading from the slave with canonical input processing enabled means
	// that we read at max until hitting a line end or EOF.
	if (!fTTY->is_master && (fTTY->settings->termios.c_lflag & ICANON) != 0) {
		return line_buffer_readable_line(fTTY->input_buffer,
			fTTY->settings->termios.c_cc[VEOL],
			fTTY->settings->termios.c_cc[VEOF]);
	}

	return line_buffer_readable(fTTY->input_buffer);
}


status_t
ReaderLocker::_CheckBackgroundRead() const
{
	// only relevant for the slave end
	if (fTTY->is_master)
		return B_OK;

	pid_t processGroup = getpgid(0);
	if (fTTY->settings->pgrp_id != 0
			&& processGroup != fTTY->settings->pgrp_id) {
		if (team_get_controlling_tty() == fTTY)
			send_signal(-processGroup, SIGTTIN);
	}

	return B_OK;
}


// #pragma mark -


static void
reset_termios(struct termios& termios)
{
	memset(&termios, 0, sizeof(struct termios));

	termios.c_iflag = ICRNL;
	termios.c_oflag = OPOST | ONLCR;
	termios.c_cflag = B19200 | CS8 | CREAD | HUPCL;
		// enable receiver, hang up on last close
	termios.c_lflag = ECHO | ISIG | ICANON;
	termios.c_ispeed = 0;
	termios.c_ospeed = 0;
	termios.c_ispeed_high = 0;
	termios.c_ospeed_high = 0;

	// control characters
	termios.c_cc[VINTR] = CTRL('C');
	termios.c_cc[VQUIT] = CTRL('\\');
	termios.c_cc[VERASE] = 0x7f;
	termios.c_cc[VKILL] = CTRL('U');
	termios.c_cc[VEOF] = CTRL('D');
	termios.c_cc[VEOL] = '\0';
	termios.c_cc[VEOL2] = '\0';
	termios.c_cc[VSTART] = CTRL('S');
	termios.c_cc[VSTOP] = CTRL('Q');
	termios.c_cc[VSUSP] = CTRL('Z');
}


static void
reset_tty_settings(tty_settings& settings)
{
	reset_termios(settings.termios);

	settings.pgrp_id = 0;
		// this value prevents any signal of being sent
	settings.session_id = -1;

	// some initial window size - the TTY in question should set these values
	settings.window_size.ws_col = 80;
	settings.window_size.ws_row = 25;
	settings.window_size.ws_xpixel = settings.window_size.ws_col * 8;
	settings.window_size.ws_ypixel = settings.window_size.ws_row * 8;
}


/*!	Processes the input character and puts it into the TTY's input buffer.
	Depending on the termios flags set, signals may be sent, the input
	character changed or removed, etc.
*/
static void
tty_input_putc_locked(struct tty* tty, int c)
{
	const termios& termios = tty->settings->termios;

	// process signals if needed
	if ((termios.c_lflag & ISIG) != 0) {
		// enable signals, process INTR, QUIT, and SUSP
		int signal = -1;

		if (c == termios.c_cc[VINTR])
			signal = SIGINT;
		else if (c == termios.c_cc[VQUIT])
			signal = SIGQUIT;
		else if (c == termios.c_cc[VSUSP])
			signal = SIGTSTP;

		// do we need to deliver a signal?
		if (signal != -1) {
			// we may have to flush the input buffer
			if ((termios.c_lflag & NOFLSH) == 0)
				clear_line_buffer(tty->input_buffer);

			if (tty->settings->pgrp_id != 0)
				send_signal(-tty->settings->pgrp_id, signal);
			return;
		}
	}

	// process special canonical input characters
	if ((termios.c_lflag & ICANON) != 0) {
		// canonical mode, process ERASE and KILL
		const cc_t* controlChars = termios.c_cc;

		if (c == controlChars[VERASE]) {
			// erase one character
			char lastChar;
			if (line_buffer_tail_getc(tty->input_buffer, &lastChar)) {
				if (lastChar == controlChars[VEOF]
					|| lastChar == controlChars[VEOL]
					|| lastChar == '\n' || lastChar == '\r') {
					// EOF or end of line -- put it back
					line_buffer_putc(tty->input_buffer, lastChar);
				}
			}
			return;
		} else if (c == controlChars[VKILL]) {
			// erase line
			char lastChar;
			while (line_buffer_tail_getc(tty->input_buffer, &lastChar)) {
				if (lastChar == controlChars[VEOF]
					|| lastChar == controlChars[VEOL]
					|| lastChar == '\n' || lastChar == '\r') {
					// EOF or end of line -- put it back
					line_buffer_putc(tty->input_buffer, lastChar);
					break;
				}
			}
			return;
		} else if (c == controlChars[VEOF]) {
			// we still write the EOF to the stream -- tty_input_read() needs
			// to recognize it
			tty->pending_eof++;
		}
	}

	// Input character conversions have already been done. What reaches this
	// point can directly be written to the line buffer.

	line_buffer_putc(tty->input_buffer, c);
}


static int32
tty_readable(struct tty* tty)
{
	if (!tty->is_master && (tty->settings->termios.c_lflag & ICANON) != 0) {
		return line_buffer_readable_line(tty->input_buffer,
			tty->settings->termios.c_cc[VEOL],
			tty->settings->termios.c_cc[VEOF]);
	}

	return line_buffer_readable(tty->input_buffer);
}


/** \brief Notify anyone waiting on events for this TTY.
 *
 * The TTY lock must be held.
 *
 * Otherwise, the select_pool may be modified or deleted (which happens automatically when the last
 * item is removed from it).
 */
static void
tty_notify_select_event(struct tty* tty, uint8 event)
{
	TRACE(("tty_notify_select_event(%p, %u)\n", tty, event));

	if (tty->select_pool)
		notify_select_event_pool(tty->select_pool, event);
}


/*!	\brief Checks whether bytes can be read from/written to the line buffer of
		   the given TTY and notifies the respective queues.

	Also sends out \c B_SELECT_READ and \c B_SELECT_WRITE events as needed.

	The TTY and the request lock must be held.

	\param tty The TTY.
	\param otherTTY The connected TTY.
*/
static void
tty_notify_if_available(struct tty* tty, struct tty* otherTTY,
	bool notifySelect)
{
	if (!tty)
		return;

	// Check, if something is readable (depending on whether canonical input
	// processing is enabled).
	int32 readable = tty_readable(tty);
	if (readable > 0) {
		// if nobody is waiting send select events, otherwise notify the waiter
		if (!tty->reader_queue.IsEmpty())
			tty->reader_queue.NotifyFirst(readable);
		else if (notifySelect)
			tty_notify_select_event(tty, B_SELECT_READ);
	}

	int32 writable = line_buffer_writable(tty->input_buffer);
	if (writable > 0) {
		// if nobody is waiting send select events, otherwise notify the waiter
		if (!tty->writer_queue.IsEmpty()) {
			tty->writer_queue.NotifyFirst(writable);
		} else if (notifySelect) {
			if (otherTTY && otherTTY->open_count > 0)
				tty_notify_select_event(otherTTY, B_SELECT_WRITE);
		}
	}
}


/*!	\brief Performs input character conversion and writes the result to
		\a buffer.
	\param tty The master tty.
	\param c The input character.
	\param buffer The buffer to which to write the converted character.
	\param _bytesNeeded The number of bytes needed in the target tty's
		line buffer.
	\return \c true, if the character shall be processed further, \c false, if
		it shall be skipped.
*/
static bool
process_input_char(struct tty* tty, char c, char* buffer,
	size_t* _bytesNeeded)
{
	const termios& termios = tty->settings->termios;
	tcflag_t flags = termios.c_iflag;

	// signals
	if (termios.c_lflag & ISIG) {
		if (c == termios.c_cc[VINTR]
			|| c == termios.c_cc[VQUIT]
			|| c == termios.c_cc[VSUSP]) {
			*buffer = c;
			*_bytesNeeded = 0;
			return true;
		}
	}

	// canonical input characters
	if (termios.c_lflag & ICANON) {
		if (c == termios.c_cc[VERASE]
			|| c == termios.c_cc[VKILL]) {
			*buffer = c;
			*_bytesNeeded = 0;
			return true;
		}
	}

	// convert chars
	if (c == '\r') {
		if (flags & IGNCR)		// ignore CR
			return false;
		if (flags & ICRNL)		// CR -> NL
			c = '\n';
	} else if (c == '\n') {
		if (flags & INLCR)		// NL -> CR
			c = '\r';
	} else if ((flags & ISTRIP)	!= 0)	// strip off eighth bit
		c &= 0x7f;

	*buffer = c;
	*_bytesNeeded = 1;
	return true;
}


/*!	\brief Performs output character conversion and writes the result to
		\a buffer.
	\param tty The master tty.
	\param c The output character.
	\param buffer The buffer to which to write the converted character(s).
	\param _bytesWritten The number of bytes written to the output buffer
		(max 3).
	\param echoed \c true if the output char to be processed has been echoed
		from the input.
*/
static void
process_output_char(struct tty* tty, char c, char* buffer,
	size_t* _bytesWritten, bool echoed)
{
	const termios& termios = tty->settings->termios;
	tcflag_t flags = termios.c_oflag;

	if (flags & OPOST) {
		if (echoed && c == termios.c_cc[VERASE]) {
			if (termios.c_lflag & ECHOE) {
				// ERASE -> BS SPACE BS
				buffer[0] = CTRL('H');
				buffer[1] = ' ';
				buffer[2] = CTRL('H');;
				*_bytesWritten = 3;
				return;
			}
		} else if (echoed && c == termios.c_cc[VKILL]) {
			if (!(termios.c_lflag & ECHOK)) {
				// don't echo KILL
				*_bytesWritten = 0;
				return;
			}
		} else if (echoed && c == termios.c_cc[VEOF]) {
			// don't echo EOF
			*_bytesWritten = 0;
			return;
		} else if (c == '\n') {
			if (echoed && !(termios.c_lflag & ECHONL)) {
				// don't echo NL
				*_bytesWritten = 0;
				return;
			}
			if (flags & ONLCR) {			// NL -> CR-NL
				buffer[0] = '\r';
				buffer[1] = '\n';
				*_bytesWritten = 2;
				return;
			}
		} else if (c == '\r') {
			if (flags & OCRNL) {			// CR -> NL
				c = '\n';
			} else if (flags & ONLRET) {	// NL also does RET, ignore CR
				*_bytesWritten = 0;
				return;
			} else if (flags & ONOCR) {		// don't output CR at column 0
				// TODO: We can't decide that here.
			}
		} else {
			if (flags & OLCUC)				// lower case -> upper case
				c = toupper(c);
		}
	}

	*buffer = c;
	*_bytesWritten = 1;
}


static status_t
tty_write_to_tty_master_unsafe(tty_cookie* sourceCookie, const char* data,
	size_t* _length)
{
	struct tty* source = sourceCookie->tty;
	struct tty* target = sourceCookie->other_tty;
	size_t length = *_length;
	size_t bytesWritten = 0;
	uint32 mode = sourceCookie->open_mode;
	bool dontBlock = (mode & O_NONBLOCK) != 0;

	// bail out, if source is already closed
	TTYReference sourceTTYReference(sourceCookie);
	if (!sourceTTYReference.IsLocked())
		return B_FILE_ERROR;

	if (length == 0)
		return B_OK;

	WriterLocker locker(sourceCookie);

	// if the target is not open, fail now
	if (target->open_count <= 0)
		return B_FILE_ERROR;

	bool echo = (source->settings->termios.c_lflag & ECHO) != 0;

	TRACE(("tty_write_to_tty_master(source = %p, target = %p, "
		"length = %lu%s)\n", source, target, length,
		(echo ? ", echo mode" : "")));

	// Make sure we are first in the writer queue(s) and AvailableBytes() is
	// initialized.
	status_t status = locker.AcquireWriter(dontBlock, 0);
	if (status != B_OK) {
		*_length = 0;
		return status;
	}
	size_t writable = locker.AvailableBytes();
	size_t writtenSinceLastNotify = 0;

	while (bytesWritten < length) {
		// fetch next char and do input processing
		char c;
		size_t bytesNeeded;
		if (!process_input_char(source, *data, &c, &bytesNeeded)) {
			// input char shall be skipped
			data++;
			bytesWritten++;
			continue;
		}

		// If in echo mode, we do the output conversion and need to update
		// the needed bytes count.
		char echoBuffer[3];
		size_t echoBytes = 0;
		if (echo) {
			process_output_char(source, c, echoBuffer, &echoBytes, true);
			if (echoBytes > bytesNeeded)
				bytesNeeded = echoBytes;
		}

		// If there's not enough space to write what we have, we need to wait
		// until it is available.
		if (writable < bytesNeeded) {
			if (writtenSinceLastNotify > 0) {
				tty_notify_if_available(target, source, true);
				if (echo)
					tty_notify_if_available(source, target, true);
				writtenSinceLastNotify = 0;
			}

			status = locker.AcquireWriter(dontBlock, bytesNeeded);
			if (status != B_OK) {
				*_length = bytesWritten;
				return *_length == 0 ? status : B_OK;
			}

			writable = locker.AvailableBytes();

			// XXX: do we need to support VMIN & VTIME for write() ?

			// We need to restart the loop, since the termios flags might have
			// changed in the meantime (while we've unlocked the tty). Note,
			// that we don't re-get "echo" -- maybe we should.
			continue;
		}

		// write the bytes
		tty_input_putc_locked(target, c);

		if (echo) {
			for (size_t i = 0; i < echoBytes; i++)
				line_buffer_putc(source->input_buffer, echoBuffer[i]);
		}

		writable -= bytesNeeded;
		data++;
		bytesWritten++;
		writtenSinceLastNotify++;
	}

	return B_OK;
}


static status_t
tty_write_to_tty_slave_unsafe(tty_cookie* sourceCookie, const char* data,
	size_t* _length)
{
	struct tty* target = sourceCookie->other_tty;
	size_t length = *_length;
	size_t bytesWritten = 0;
	uint32 mode = sourceCookie->open_mode;
	bool dontBlock = (mode & O_NONBLOCK) != 0;

	// bail out, if source is already closed
	TTYReference sourceTTYReference(sourceCookie);
	if (!sourceTTYReference.IsLocked())
		return B_FILE_ERROR;

	if (length == 0)
		return B_OK;

	WriterLocker locker(sourceCookie);

	// if the target is not open, fail now
	if (target->open_count <= 0)
		return B_FILE_ERROR;

	TRACE(("tty_write_to_tty_slave(source = %p, target = %p, length = %lu)\n",
		sourceCookie->tty, target, length));

	// Make sure we are first in the writer queue(s) and AvailableBytes() is
	// initialized.
	status_t status = locker.AcquireWriter(dontBlock, 0);
	if (status != B_OK) {
		*_length = 0;
		return status;
	}
	size_t writable = locker.AvailableBytes();
	size_t writtenSinceLastNotify = 0;

	while (bytesWritten < length) {
		// fetch next char and do output processing
		char buffer[3];
		size_t bytesNeeded;
		process_output_char(target, *data, buffer, &bytesNeeded, false);

		// If there's not enough space to write what we have, we need to wait
		// until it is available.
		if (writable < bytesNeeded) {
			if (writtenSinceLastNotify > 0) {
				tty_notify_if_available(target, sourceCookie->tty, true);
				writtenSinceLastNotify = 0;
			}

			status = locker.AcquireWriter(dontBlock, bytesNeeded);
			if (status != B_OK) {
				*_length = bytesWritten;
				return *_length == 0 ? status : B_OK;
			}

			writable = locker.AvailableBytes();

			// We need to restart the loop, since the termios flags might have
			// changed in the meantime (while we've unlocked the tty).
			continue;
		}

		// write the bytes
		for (size_t i = 0; i < bytesNeeded; i++)
			line_buffer_putc(target->input_buffer, buffer[i]);

		writable -= bytesNeeded;
		data++;
		bytesWritten++;
		writtenSinceLastNotify++;
	}

	return B_OK;
}


static status_t
tty_write_to_tty_master(tty_cookie* sourceCookie, const void* _buffer,
	size_t* _length)
{
	const char* buffer = (const char*)_buffer;
	size_t bytesRemaining = *_length;
	*_length = 0;

	while (bytesRemaining > 0) {
		// copy data to stack
		char safeBuffer[256];
		size_t toWrite = min_c(sizeof(safeBuffer), bytesRemaining);
		status_t error = user_memcpy(safeBuffer, buffer, toWrite);
		if (error != B_OK)
			return error;

		// write them
		size_t written = toWrite;
		error = tty_write_to_tty_master_unsafe(sourceCookie, safeBuffer,
			&written);
		if (error != B_OK)
			return error;

		buffer += written;
		bytesRemaining -= written;
		*_length += written;

		if (written < toWrite)
			return B_OK;
	}

	return B_OK;
}


static status_t
tty_write_to_tty_slave(tty_cookie* sourceCookie, const void* _buffer,
	size_t* _length)
{
	const char* buffer = (const char*)_buffer;
	size_t bytesRemaining = *_length;
	*_length = 0;

	while (bytesRemaining > 0) {
		// copy data to stack
		char safeBuffer[256];
		size_t toWrite = min_c(sizeof(safeBuffer), bytesRemaining);
		status_t status = user_memcpy(safeBuffer, buffer, toWrite);
		if (status != B_OK)
			return status;

		// write them
		size_t written = toWrite;
		status = tty_write_to_tty_slave_unsafe(sourceCookie, safeBuffer,
			&written);
		if (status != B_OK)
			return *_length == 0 ? status : B_OK;

		buffer += written;
		bytesRemaining -= written;
		*_length += written;

		if (written < toWrite)
			return B_OK;
	}

	return B_OK;
}


// #pragma mark - public API


status_t
tty_create(tty_service_func func, struct tty* master, struct tty** _tty)
{
	struct tty* tty = new(std::nothrow) (struct tty);
	if (tty == NULL)
		return B_NO_MEMORY;

	if (master == NULL) {
		tty->is_master = true;
		tty->lock = new(std::nothrow) recursive_lock;
		tty->settings = new(std::nothrow) tty_settings;
		if (tty->lock == NULL || tty->settings == NULL) {
			delete tty->lock;
			delete tty->settings;
			delete tty;
			return B_NO_MEMORY;
		}

		recursive_lock_init(tty->lock, "tty lock");
		reset_tty_settings(*tty->settings);
	} else {
		tty->is_master = false;
		tty->lock = master->lock;
		tty->settings = master->settings;
	}

	tty->ref_count = 0;
	tty->open_count = 0;
	tty->opened_count = 0;
	tty->select_pool = NULL;
	tty->pending_eof = 0;
	tty->hardware_bits = 0;
	tty->is_exclusive = false;

	status_t status = init_line_buffer(tty->input_buffer, TTY_BUFFER_SIZE);

	if (status < B_OK) {
		if (tty->is_master) {
			recursive_lock_destroy(tty->lock);
			delete tty->lock;
		}
		delete tty;
		return status;
	}

	tty->service_func = func;

	*_tty = tty;

	return B_OK;
}


void
tty_destroy(struct tty* tty)
{
	TRACE(("tty_destroy(%p)\n", tty));
	ASSERT(tty->ref_count == 0);

	uninit_line_buffer(tty->input_buffer);
	delete_select_sync_pool(tty->select_pool);
	if (tty->is_master) {
		recursive_lock_destroy(tty->lock);
		delete tty->lock;
		delete tty->settings;
	}

	delete tty;
}


status_t
tty_create_cookie(struct tty* tty, struct tty* otherTTY, uint32 openMode, tty_cookie** _cookie)
{
	tty_cookie* cookie = new(std::nothrow) tty_cookie;
	if (cookie == NULL)
		return B_NO_MEMORY;

	cookie->blocking_semaphore = create_sem(0, "wait for tty close");
	if (cookie->blocking_semaphore < 0) {
		status_t status = cookie->blocking_semaphore;
		delete cookie;
		return status;
	}

	cookie->tty = tty;
	cookie->other_tty = otherTTY;
	cookie->open_mode = openMode;
	cookie->thread_count = 0;
	cookie->closed = false;


	RecursiveLocker locker(cookie->tty->lock);

	if (tty->is_exclusive && geteuid() != 0) {
		delete_sem(cookie->blocking_semaphore);
		delete cookie;
		return B_BUSY;
	}

	// add to the TTY's cookie list
	tty->cookies.Add(cookie);
	tty->open_count++;
	tty->ref_count++;
	tty->opened_count++;

	*_cookie = cookie;

	return B_OK;
}


void
tty_close_cookie(tty_cookie* cookie)
{
	MutexLocker locker(gTTYCookieLock);

	// Already closed? This can happen for slaves that have been closed when
	// the master was closed.
	if (cookie->closed)
		return;

	// set the cookie's `closed' flag
	cookie->closed = true;
	bool unblock = (cookie->thread_count > 0);

	// unblock blocking threads
	if (unblock) {
		cookie->tty->reader_queue.NotifyError(cookie, B_FILE_ERROR);
		cookie->tty->writer_queue.NotifyError(cookie, B_FILE_ERROR);

		if (cookie->other_tty->open_count > 0) {
			cookie->other_tty->reader_queue.NotifyError(cookie, B_FILE_ERROR);
			cookie->other_tty->writer_queue.NotifyError(cookie, B_FILE_ERROR);
		}
	}

	locker.Unlock();

	// wait till all blocking (and now unblocked) threads have left the
	// critical code
	if (unblock) {
		TRACE(("tty_close_cookie(): cookie %p, there're still pending "
			"operations, acquire blocking sem %" B_PRId32 "\n", cookie,
			cookie->blocking_semaphore));

		acquire_sem(cookie->blocking_semaphore);
	}

	// For the removal of the cookie acquire the TTY's lock. This ensures, that
	// cookies will not be removed from a TTY (or added -- cf. add_tty_cookie())
	// as long as the TTY's lock is being held.
	RecursiveLocker ttyLocker(cookie->tty->lock);

	// remove the cookie from the TTY's cookie list
	cookie->tty->cookies.Remove(cookie);

	// close the tty, if no longer used
	if (--cookie->tty->open_count == 0) {
		// The last cookie of this tty has been closed. We're going to close
		// the TTY and need to unblock all write requests before. There should
		// be no read requests, since only a cookie of this TTY issues those.
		// We do this by first notifying all queued requests of the error
		// condition. We then clear the line buffer for the TTY and queue
		// an own request.

		// Notify the other TTY first; it doesn't accept any read/writes
		// while there is only one end.
		cookie->other_tty->reader_queue.NotifyError(B_FILE_ERROR);
		cookie->other_tty->writer_queue.NotifyError(B_FILE_ERROR);

		RecursiveLocker requestLocker(gTTYRequestLock);

		// we only need to do all this, if the writer queue is not empty
		if (!cookie->tty->writer_queue.IsEmpty()) {
	 		// notify the blocking writers
			cookie->tty->writer_queue.NotifyError(B_FILE_ERROR);

			// enqueue our request
			RequestOwner requestOwner;
			requestOwner.Enqueue(cookie, &cookie->tty->writer_queue);

			requestLocker.Unlock();

			// clear the line buffer
			clear_line_buffer(cookie->tty->input_buffer);

			ttyLocker.Unlock();

			// wait for our turn
			requestOwner.Wait(false);

			// re-lock
			ttyLocker.Lock();
			requestLocker.Lock();

			// dequeue our request
			requestOwner.Dequeue();
		}

		requestLocker.Unlock();

		// notify a select read and write event on the other tty, if we've closed this tty
		if (cookie->other_tty->open_count > 0) {
			tty_notify_select_event(cookie->other_tty, B_SELECT_WRITE);
			tty_notify_select_event(cookie->other_tty, B_SELECT_READ);
		}

		cookie->tty->is_exclusive = false;
	}
}


void
tty_destroy_cookie(tty_cookie* cookie)
{
	RecursiveLocker locker(cookie->tty->lock);
	cookie->tty->ref_count--;
	locker.Unlock();

	if (cookie->blocking_semaphore >= 0)
		delete_sem(cookie->blocking_semaphore);

	delete cookie;
}


status_t
tty_read(tty_cookie* cookie, void* _buffer, size_t* _length)
{
	char* buffer = (char*)_buffer;
	struct tty* tty = cookie->tty;
	uint32 mode = cookie->open_mode;
	bool dontBlock = (mode & O_NONBLOCK) != 0;
	size_t length = *_length;
	bool canon = true;
	bigtime_t timeout = dontBlock ? 0 : B_INFINITE_TIMEOUT;
	bigtime_t interCharTimeout = 0;
	size_t bytesNeeded = 1;

	TRACE(("tty_input_read(tty = %p, length = %lu, mode = %" B_PRIu32 ")\n",
		tty, length, mode));

	if (length == 0)
		return B_OK;

	// bail out, if the TTY is already closed
	TTYReference ttyReference(cookie);
	if (!ttyReference.IsLocked())
		return B_FILE_ERROR;

	ReaderLocker locker(cookie);

	// handle raw mode
	if ((!tty->is_master) && ((tty->settings->termios.c_lflag & ICANON) == 0)) {
		canon = false;
		if (!dontBlock) {
			// Non-blocking mode. Handle VMIN and VTIME.
			bytesNeeded = tty->settings->termios.c_cc[VMIN];
			bigtime_t vtime = tty->settings->termios.c_cc[VTIME] * 100000;
			TRACE(("tty_input_read: icanon vmin %lu, vtime %" B_PRIdBIGTIME
				"us\n", bytesNeeded, vtime));

			if (bytesNeeded == 0) {
				// In this case VTIME specifies a relative total timeout. We
				// have no inter-char timeout, though.
				timeout = vtime;
			} else {
				// VTIME specifies the inter-char timeout. 0 is indefinitely.
				if (vtime == 0)
					interCharTimeout = B_INFINITE_TIMEOUT;
				else
					interCharTimeout = vtime;

				if (bytesNeeded > length)
					bytesNeeded = length;
			}
		}
	}

	status_t status;
	*_length = 0;

	do {
		TRACE(("tty_input_read: AcquireReader(%" B_PRIdBIGTIME "us, %ld)\n",
			timeout, bytesNeeded));
		status = locker.AcquireReader(timeout, bytesNeeded);
		size_t toRead = locker.AvailableBytes();
		if (status != B_OK && toRead == 0) {
			TRACE(("tty_input_read() AcquireReader failed\n"));
			break;
		}

		if (toRead > length)
			toRead = length;

		bool _hitEOF = false;
		bool* hitEOF = canon && tty->pending_eof > 0 ? &_hitEOF : NULL;

		ssize_t bytesRead = line_buffer_user_read(tty->input_buffer, buffer,
			toRead, tty->settings->termios.c_cc[VEOF], hitEOF);
		if (bytesRead < 0) {
			status = bytesRead;
			break;
		}

		buffer += bytesRead;
		length -= bytesRead;
		*_length += bytesRead;
		bytesNeeded = (size_t)bytesRead > bytesNeeded
			? 0 : bytesNeeded - bytesRead;

		// we hit an EOF char -- bail out, whatever amount of data we have
		if (hitEOF && *hitEOF) {
			tty->pending_eof--;
			break;
		}

		// Once we have read something reset the timeout to the inter-char
		// timeout, if applicable.
		if (!dontBlock && !canon && *_length > 0)
			timeout = interCharTimeout;
	} while (bytesNeeded > 0);

	if (status == B_WOULD_BLOCK || status == B_TIMED_OUT) {
		// In non-blocking non-canonical-input-processing mode never return
		// timeout errors. Just return 0, if nothing has been read.
		if (!dontBlock && !canon)
			status = B_OK;
	}

	TRACE(("tty_input_read() status 0x%" B_PRIx32 "\n", status));

	return *_length == 0 ? status : B_OK;
}


status_t
tty_write(tty_cookie* sourceCookie, const void* _buffer, size_t* _length)
{
	if (sourceCookie->tty->is_master)
		return tty_write_to_tty_master(sourceCookie, _buffer, _length);

	return tty_write_to_tty_slave(sourceCookie, _buffer, _length);
}


status_t
tty_control(tty_cookie* cookie, uint32 op, void* buffer, size_t length)
{
	struct tty* tty = cookie->tty;

	// bail out, if already closed
	TTYReference ttyReference(cookie);
	if (!ttyReference.IsLocked())
		return B_FILE_ERROR;

	TRACE(("tty_ioctl: tty %p, op %" B_PRIu32 ", buffer %p, length %"
		B_PRIuSIZE "\n", tty, op, buffer, length));
	RecursiveLocker locker(tty->lock);

	switch (op) {
		// blocking/non-blocking mode

		case B_SET_BLOCKING_IO:
			cookie->open_mode &= ~O_NONBLOCK;
			return B_OK;
		case B_SET_NONBLOCKING_IO:
			cookie->open_mode |= O_NONBLOCK;
			return B_OK;

		// get and set TTY attributes

		case TCGETA:
			TRACE(("tty: get attributes\n"));
			return user_memcpy(buffer, &tty->settings->termios,
				sizeof(struct termios));

		case TCSETA:
		case TCSETAW:
		case TCSETAF:
		{
			TRACE(("tty: set attributes (iflag = %" B_PRIx32 ", oflag = %"
				B_PRIx32 ", cflag = %" B_PRIx32 ", lflag = %" B_PRIx32 ")\n",
				tty->settings->termios.c_iflag, tty->settings->termios.c_oflag,
				tty->settings->termios.c_cflag,
				tty->settings->termios.c_lflag));

			status_t status = user_memcpy(&tty->settings->termios, buffer,
				sizeof(struct termios));
			if (status != B_OK)
				return status;

			tty->service_func(tty, TTYSETMODES, &tty->settings->termios,
				sizeof(struct termios));
			return status;
		}

		// get and set process group ID

		case TIOCGPGRP:
			TRACE(("tty: get pgrp_id\n"));
			return user_memcpy(buffer, &tty->settings->pgrp_id, sizeof(pid_t));

		case TIOCSPGRP:
		{
			TRACE(("tty: set pgrp_id\n"));
			pid_t groupID;

			if (user_memcpy(&groupID, buffer, sizeof(pid_t)) != B_OK)
				return B_BAD_ADDRESS;

			status_t error = team_set_foreground_process_group(tty,
				groupID);
			if (error == B_OK)
				tty->settings->pgrp_id = groupID;
			return error;
		}

		// become controlling TTY
		case TIOCSCTTY:
		{
			TRACE(("tty: become controlling tty\n"));
			pid_t processID = getpid();
			pid_t sessionID = getsid(processID);
			// Only session leaders can become controlling tty
			if (processID != sessionID)
				return B_NOT_ALLOWED;
			// Check if already controlling tty
			if (team_get_controlling_tty() == tty)
				return B_OK;
			tty->settings->session_id = sessionID;
			tty->settings->pgrp_id = sessionID;
			team_set_controlling_tty(tty);

			// NOTE: We do not acquire a reference to the TTY for the team, so
			// consumers of team_get_controlling_tty() must check the TTY is still
			// valid before actually using it!
			return B_OK;
		}

		// get session leader process group ID
		case TIOCGSID:
		{
			TRACE(("tty: get session_id\n"));
			return user_memcpy(buffer, &tty->settings->session_id,
				sizeof(pid_t));
		}

		// get and set window size

		case TIOCGWINSZ:
			TRACE(("tty: get window size\n"));
			return user_memcpy(buffer, &tty->settings->window_size,
				sizeof(struct winsize));

		case TIOCSWINSZ:
		{
			uint16 oldColumns = tty->settings->window_size.ws_col;
			uint16 oldRows = tty->settings->window_size.ws_row;

			TRACE(("tty: set window size\n"));
			if (user_memcpy(&tty->settings->window_size, buffer,
					sizeof(struct winsize)) < B_OK) {
				return B_BAD_ADDRESS;
			}

			// send a signal only if the window size has changed
			if ((oldColumns != tty->settings->window_size.ws_col
					|| oldRows != tty->settings->window_size.ws_row)
				&& tty->settings->pgrp_id != 0) {
				send_signal(-tty->settings->pgrp_id, SIGWINCH);
			}

			return B_OK;
		}

		case 'ichr':			// BeOS (int*) (pre- select() support)
		{
			int wanted;
			int toRead;

			// help identify apps using it
			//dprintf("tty: warning: legacy BeOS opcode 'ichr'\n");

			if (user_memcpy(&wanted, buffer, sizeof(int)) != B_OK)
				return B_BAD_ADDRESS;

			// release the mutex and grab a read lock
			locker.Unlock();
			ReaderLocker readLocker(cookie);

			bigtime_t timeout = wanted == 0 ? 0 : B_INFINITE_TIMEOUT;

			// TODO: If wanted is > the TTY buffer size, this loop cannot work
			// correctly. Refactor the read code!
			do {
				status_t status = readLocker.AcquireReader(timeout, wanted);
				if (status != B_OK)
					return status;

				toRead = readLocker.AvailableBytes();
			} while (toRead < wanted);

			if (user_memcpy(buffer, &toRead, sizeof(int)) != B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		}

		case FIONREAD:
		{
			int toRead = 0;

			// release the mutex and grab a read lock
			locker.Unlock();
			ReaderLocker readLocker(cookie);

			status_t status = readLocker.AcquireReader(0, 1);
			if (status == B_OK)
				toRead = readLocker.AvailableBytes();
			else if (status != B_WOULD_BLOCK)
				return status;

			if (user_memcpy(buffer, &toRead, sizeof(int)) != B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		}

		case TIOCOUTQ:
		{
			int toWrite = 0;

			// release the mutex and grab a write lock
			locker.Unlock();
			WriterLocker writeLocker(cookie);

			status_t status = writeLocker.AcquireWriter(0, 1);
			if (status == B_OK)
				toWrite = line_buffer_readable(tty->input_buffer);
			else if (status != B_WOULD_BLOCK)
				return status;

			if (user_memcpy(buffer, &toWrite, sizeof(int)) != B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		}

		case TCXONC:			// Unix, but even Linux doesn't handle it
			//dprintf("tty: unsupported TCXONC\n");
			break;

		case TCSETDTR:
		case TCSETRTS:
		case TIOCMSET:
		{
			// control line state setting, we only support DTR and RTS
			int value;
			if (user_memcpy(&value, buffer, sizeof(value)) != B_OK)
				return B_BAD_ADDRESS;

			bool result = true;
			bool dtr = (op == TCSETDTR  && value != 0)
				|| (op == TIOCMSET && (value & TIOCM_DTR) != 0);
			if (op == TCSETDTR || op == TIOCMSET)
				result &= tty->service_func(tty, TTYSETDTR, &dtr, sizeof(dtr));

			bool rts = (op == TCSETRTS && value != 0)
				|| (op == TIOCMSET && (value & TIOCM_RTS) != 0);
			if (op == TCSETRTS || op == TIOCMSET)
				result &= tty->service_func(tty, TTYSETRTS, &rts, sizeof(rts));

			return result ? B_OK : B_ERROR;
		}

		case TCGETBITS:
		case TIOCMGET:
		{
			tty->service_func(tty, TTYGETSIGNALS, NULL, 0);
			int bits = tty->hardware_bits;
			return user_memcpy(buffer, &bits, sizeof(bits));
		}

		case TIOCMBIS:
		case TIOCMBIC:
		{
			// control line state setting, we only support DTR and RTS
			int value;
			if (user_memcpy(&value, buffer, sizeof(value)) != B_OK)
				return B_BAD_ADDRESS;

			bool result = true;
			bool dtr = (op == TIOCMBIS);
			if (value & TIOCM_DTR)
				result &= tty->service_func(tty, TTYSETDTR, &dtr, sizeof(dtr));

			bool rts = (op == TIOCMBIS);
			if (value & TIOCM_RTS)
				result &= tty->service_func(tty, TTYSETRTS, &rts, sizeof(rts));

			return result ? B_OK : B_ERROR;
		}

		case TIOCSBRK:
		case TIOCCBRK:
		case TCSBRK:
		{
			bool set;
			if (op == TIOCSBRK)
				set = true;
			else if (op == TIOCCBRK)
				set = false;
			else {
				int value = (int)(uintptr_t)buffer;
				set = value != 0;
			}

			if (tty->service_func(tty, TTYSETBREAK, &set, sizeof(set)))
				return B_OK;

			return B_ERROR;
		}

		case TCFLSH:
		{
			int value = (int)(uintptr_t)buffer;
			if (value & TCOFLUSH) {
				struct tty* otherTTY = cookie->other_tty;
				if (otherTTY->open_count <= 0)
					return B_ERROR;

				clear_line_buffer(otherTTY->input_buffer);
			}

			if (value & TCIFLUSH)
				clear_line_buffer(tty->input_buffer);

			if (tty->service_func(tty, TTYFLUSH, &value, sizeof(value)))
				return B_OK;

			return B_ERROR;
		}

		case TIOCEXCL:
		{
			tty->is_exclusive = true;
			return B_OK;
		}

		case TIOCNXCL:
		{
			tty->is_exclusive = false;
			return B_OK;
		}
	}

	TRACE(("tty: unsupported opcode %" B_PRIu32 "\n", op));
	return B_BAD_VALUE;
}


status_t
tty_select(tty_cookie* cookie, uint8 event, uint32 ref, selectsync* sync)
{
	struct tty* tty = cookie->tty;

	TRACE(("tty_select(cookie = %p, event = %u, ref = %" B_PRIu32 ", sync = "
		"%p)\n", cookie, event, ref, sync));

	// we don't support all kinds of events
	if (event < B_SELECT_READ || event > B_SELECT_ERROR)
		return B_BAD_VALUE;

	// if the TTY is already closed, we notify immediately
	TTYReference ttyReference(cookie);
	if (!ttyReference.IsLocked()) {
		TRACE(("tty_select() done: cookie %p already closed\n", cookie));

		notify_select_event(sync, event);
		return B_OK;
	}

	// lock the TTY (allows us to freely access the cookie lists of this and
	// the other TTY)
	RecursiveLocker ttyLocker(tty->lock);

	// get the other TTY -- needed for `write' events
	struct tty* otherTTY = cookie->other_tty;
	if (otherTTY->open_count <= 0)
		otherTTY = NULL;

	// add the event to the TTY's pool
	status_t error = add_select_sync_pool_entry(&tty->select_pool, sync, event);
	if (error != B_OK) {
		TRACE(("tty_select() done: add_select_sync_pool_entry() failed: %"
			B_PRIx32 "\n", error));

		return error;
	}

	// finally also acquire the request mutex, for access to the reader/writer
	// queues
	RecursiveLocker requestLocker(gTTYRequestLock);

	// check, if the event is already present
	switch (event) {
		case B_SELECT_READ:
			if (tty->reader_queue.IsEmpty() && tty_readable(tty) > 0)
				notify_select_event(sync, event);
			break;

		case B_SELECT_WRITE:
		{
			// writes go to the other TTY
			if (!otherTTY) {
				notify_select_event(sync, event);
				break;
			}

			// In case input is echoed, we have to check, whether we can
			// currently can write to our TTY as well.
			bool echo = (tty->is_master
				&& tty->settings->termios.c_lflag & ECHO);

			if (otherTTY->writer_queue.IsEmpty()
				&& line_buffer_writable(otherTTY->input_buffer) > 0) {
				if (!echo
					|| (tty->writer_queue.IsEmpty()
						&& line_buffer_writable(tty->input_buffer) > 0)) {
					notify_select_event(sync, event);
				}
			}
			break;
		}

		case B_SELECT_ERROR:
		default:
			break;
	}

	return B_OK;
}


status_t
tty_deselect(tty_cookie* cookie, uint8 event, selectsync* sync)
{
	struct tty* tty = cookie->tty;

	TRACE(("tty_deselect(cookie = %p, event = %u, sync = %p)\n", cookie, event,
		sync));

	// we don't support all kinds of events
	if (event < B_SELECT_READ || event > B_SELECT_ERROR)
		return B_BAD_VALUE;

	// lock the TTY (guards the select sync pool, among other things)
	RecursiveLocker ttyLocker(tty->lock);

	return remove_select_sync_pool_entry(&tty->select_pool, sync, event);
}


status_t
tty_hardware_signal(tty_cookie* cookie, int signal, bool set)
{
	int bit = 0;

	switch (signal) {
		case TTYHWDCD:
			bit = TCGB_DCD;
			break;
		case TTYHWCTS:
			bit = TCGB_CTS;
			break;
		case TTYHWDSR:
			bit = TCGB_DSR;
			break;
		case TTYHWRI:
			bit = TCGB_RI;
			break;

		default:
			return B_BAD_VALUE;
	}

	if (set)
		cookie->tty->hardware_bits |= bit;
	else
		cookie->tty->hardware_bits &= ~bit;

	return B_ERROR;
}
