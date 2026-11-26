// ChatListBox.h — engine chat-widget object behind ChatHistoryWidget.
// See ChatListBox.cpp for the rationale and full vtable layout.
#pragma once

extern "C" {
// Allocate, default-construct, return the chat-widget object.  Caller stores
// the pointer into ChatHistoryWidget.  Returns NULL on OOM.
void* ChatListBox_Construct(void);

// Vtable del widget. El puntero vive en una cabecera nativa delante del objeto.
void** ChatWidget_GetVTable(void* widget);

// Widget hover flag — set by ChatListBox per-frame input handling, consumed
// by cursor render.  (Storage in ChatListBox.cpp.)
extern int MouseOnWindow;
}
