/************************************************
* COPYRIGHT 2016.
*
* NAME: ctl_event.c
*
* DESCRIPTION:
*   define some function interfaces of event module
*
* REVISION HISTORY:
*	Created by Kevin Shi at 05/05/2016
*
* END OF FILE HEADER
************************************************/
#include "CtlEvent.h"
#include "assert.h"
#include "Common.h"
/************************************************
* Declaration
************************************************/
#define EVENT_MAX_NUM 50

/************************************************
* Variable 
************************************************/
//event queue, store TE_EVENT
static TS_EVENT g_event_queue[EVENT_MAX_NUM] = {E_EVENT_NULL};
//the first event index, range:0 - (EVENT_MAX_NUM-1)
static T_S32 g_event_queue_begin_index = 0;
//the index after last event, range:0 - (EVENT_MAX_NUM-1)
static T_S32 g_event_queue_end_index = 0;
static TF_EVENT_FUNC g_event_handle_fun_arr[E_EVENT_MAX];

/************************************************
* Function 
************************************************/

/************************************************
* COPYRIGHT 2016.
*
* DESCRIPTION:
*   get event, if no event, will return E_EVENT_NULL
*
* PROTOTYPE: 
*   void CtlEventGet(TS_EVENT *event_obj);
*
* ARGUMENT:
*   event_obj[OUT]: 
*
* RETURN:
*   
* END OF FUNCTION HEADER
************************************************/
void CtlEventGet(TS_EVENT *event_obj)
{
    T_S32 end_index = g_event_queue_end_index;
    *event_obj = g_event_queue[g_event_queue_begin_index];
    //if no event exist, set the return value as E_EVENT_NULL
    if(g_event_queue_begin_index == end_index)
    {
        event_obj->event = E_EVENT_NULL;
    }
    else
    {
        //set current event as null
        g_event_queue[g_event_queue_begin_index].event = E_EVENT_NULL;
        if(g_event_queue_begin_index<EVENT_MAX_NUM-1)
        {
            //switch to next event index
            g_event_queue_begin_index++;
        }
        //if reach max, go back to the first
        else
        {
            g_event_queue_begin_index = 0;
        }
    }
}

/************************************************
* COPYRIGHT 2016.
*
* DESCRIPTION:
*   handle event
*
* PROTOTYPE: 
*   void CtlEventHandle(const TS_EVENT &event_obj);
*
* ARGUMENT:
*   event_obj[IN]: 
*
* RETURN:
*   
* END OF FUNCTION HEADER
************************************************/
void CtlEventHandle(TS_EVENT *event_obj)
{
    //if event and the handle is not null, call the handle
    if (event_obj->event!=E_EVENT_NULL && g_event_handle_fun_arr[event_obj->event])
    {
        g_event_handle_fun_arr[event_obj->event](event_obj->param);
    }
}

/************************************************
* COPYRIGHT 2016.
*
* DESCRIPTION:
*   handle all events until no event
*
* PROTOTYPE: 
*   void CtlEventHandleAll();
*
* ARGUMENT:
*
* RETURN:
*   
* END OF FUNCTION HEADER
************************************************/
void CtlEventHandleAll()
{
    TS_EVENT event;

    CtlEventGet(&event);
    //if exist event, handle it
    while (event.event != E_EVENT_NULL)
    {
        CtlEventHandle(&event);
        CtlEventGet(&event);
    }
}

/************************************************
* COPYRIGHT 2016.
*
* DESCRIPTION:
*   post event then return immediately
*
* PROTOTYPE: 
*   void CtlEventPost(TE_EVENT event, T_UINT param);
*
* ARGUMENT:
*   event: 
*   param:
*
* RETURN:
*
* END OF FUNCTION HEADER
************************************************/
void CtlEventPost(TE_EVENT event, T_U32 param)
{
    T_S32 next_index;
    M_Assert(event<E_EVENT_MAX);

    //InterruptCtlEnable();

    next_index = g_event_queue_end_index;
    //move the end index to next
    next_index++;
    if (next_index >= EVENT_MAX_NUM)
    {
        next_index = 0;
    }
    //if the next index is same as begin index, it indicate that the event queue is full
    if(next_index == g_event_queue_begin_index)
    {
        M_Assert("event is full!");
        while(1){}; //lint !e527
        //return;
    }
    else
    {
        //add the event to queue
        g_event_queue[g_event_queue_end_index].event = event;
        g_event_queue[g_event_queue_end_index].param = param;
        g_event_queue_end_index = next_index;
    }
    //InterruptCtlDisable();

}

/************************************************
* COPYRIGHT 2016.
*
* DESCRIPTION:
*   bind handle function for event
*
* PROTOTYPE: 
*   void CtlEventBindHandle( TE_EVENT event, TF_EVENT_FUNC func );
*
* ARGUMENT:
*   event: 
*   func:
*
* RETURN:
*
* END OF FUNCTION HEADER
************************************************/
void CtlEventBindHandle( TE_EVENT event, TF_EVENT_FUNC func )
{
    g_event_handle_fun_arr[event] = func;
}

/************************************************
* COPYRIGHT 2016.
*
* DESCRIPTION:
*   clear one event's handle
*
* PROTOTYPE: 
*   void CtlEventClearAllHandle();
*
* ARGUMENT:
*
* RETURN:
*
* END OF FUNCTION HEADER
************************************************/
void CtlEventClearHandle(TE_EVENT event)
{
    g_event_handle_fun_arr[event] = 0;
}

/************************************************
* COPYRIGHT 2016.
*
* DESCRIPTION:
*   clear all events' handle
*
* PROTOTYPE: 
*   void CtlEventClearAllHandle();
*
* ARGUMENT:
*
* RETURN:
*
* END OF FUNCTION HEADER
************************************************/
void CtlEventClearAllHandle(void)
{
    T_S32 i;
    //clear all event handle
    for(i=0; i<E_EVENT_MAX; i++)
    {
        g_event_handle_fun_arr[(TE_EVENT)i] = 0;
    }
}

/************************************************
* COPYRIGHT 2016.
*
* DESCRIPTION:
*   is exist event
*
* PROTOTYPE: 
*   TE_BOOLEAN CtlEventIsEmpty(void);
*
* ARGUMENT:
*
* RETURN:
*   E_TRUE: exist event
*   E_FALSE: not exist event
*
* END OF FUNCTION HEADER
************************************************/
TE_BOOLEAN CtlEventIsEmpty(void)
{
    //check if end index is same as begin index
    if(g_event_queue_begin_index == g_event_queue_end_index)
    {
        //no event exist
        return E_TRUE;
    }
    else
    {
        //there are events exist.
        return E_FALSE;
    }
}

