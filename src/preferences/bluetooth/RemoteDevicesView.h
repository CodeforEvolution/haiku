/*
 * Copyright 2008-09, Oliver Ruiz Dorantes, <oliver.ruiz.dorantes_at_gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef REMOTE_DEVICES_VIEW_H_
#define REMOTE_DEVICES_VIEW_H_

#include <View.h>
#include <ColorControl.h>
#include <Message.h>
#include <ListItem.h>
#include <ListView.h>
#include <Button.h>
#include <ScrollView.h>
#include <ScrollBar.h>
#include <String.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <StringView.h>
#include <Invoker.h>


class RemoteDevicesView : public BView {
public:
			RemoteDevicesView(const char *name);
			~RemoteDevicesView();
	void	AttachedToWindow();
	void	MessageReceived(BMessage *msg);

	void	LoadSettings();
	bool	IsDefaultable();

private:
	void	SetCurrentColor(rgb_color color);
	void	UpdateControls();
	void	UpdateAllColors();

private:
	BButton*		fAddButton;
	BButton*		fRemoveButton;
	BButton*		fPairButton;
	BButton*		fDisconnectButton;
//	BButton*		fBlockButton;
//	BButton*		fAvailButton;
	BListView*		fDeviceList;
	BScrollView*	fScrollView;
};

#endif
