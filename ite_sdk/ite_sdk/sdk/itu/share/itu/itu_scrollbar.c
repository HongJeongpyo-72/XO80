#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ite/itu.h"
#include "itu_cfg.h"

static const char scrollBarName[] = "ITUScrollBar";

//========================================================
//chpark.24.08.06
static int m_nTotalUser=0;
static int m_nPageCnt=0;
static int m_nBar_StartPos_Y=0;

void ituScrollBar_SetInfo(int nTotalUser, int nPageCnt)
{
	m_nTotalUser = nTotalUser;
    m_nPageCnt   = nPageCnt;
	//printf("[ituScrollBar_SetInfo] nTotalUser=%d nPageCnt=%d\n",nTotalUser,nPageCnt);
}
//========================================================


bool ituScrollBarUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    ITUScrollBar* bar = (ITUScrollBar*) widget;
    assert(bar);

    if (ev == ITU_EVENT_MOUSEDOWN)
    {
        if (ituWidgetIsEnabled(widget))
        {
            int x = arg2 - widget->rect.x;
            int y = arg3 - widget->rect.y;

            if (ituWidgetIsInside(widget, x, y))
            {
                if (bar->layout == ITU_LAYOUT_HORIZONTAL)
                {
                    int x = arg2 - widget->rect.x;
                    int pos = bar->length * x / widget->rect.width + 1;
                   

                    if (pos > bar->length)
                        pos = bar->length;

                    if (pos != bar->pos)
                    {
                        ituScrollBarSetPosition(bar, pos);
                        ituExecActions((ITUWidget*)bar, bar->actions, ITU_EVENT_CHANGED, pos);
                        ituScrollBarOnPositionChanged(bar, pos, true);
                    }
                }
                else // if (bar->layout == ITU_LAYOUT_VERTICAL)
                {
                    //int y = arg3 - widget->rect.y;
                    //int pos = bar->length * y / widget->rect.height + 1;

                     //bar의 길이
                double d_height = (double)(widget->rect.height/m_nTotalUser)*m_nPageCnt; //chpark.24.08.06
                
                //double c_height = ceil(d_height); //chpark.24.08.06
                //페이지가 증가하거나 감소할 때 마다 실질적으로 이동하는 바의 움직임 길이 
                double c_height = ceil( (widget->rect.height-d_height)/(m_nTotalUser-m_nPageCnt)); //chpark.24.08.06

				int y1 = 0; // bar의 시작 위치
                y1 = (bar->pos - 1)*c_height;

				m_nBar_StartPos_Y = y1; //chpark.24.08.06

                    //chpark.24.08.06
                    int height = (widget->rect.height/m_nTotalUser)*m_nPageCnt; //chpark.24.08.06
                    int y = arg3 - widget->rect.y;    
                    int pos = bar->length * y / (widget->rect.height - height) + 1;//한칸의 크기.  

                    //printf("[scroll bar] y=%d m_nBar_StartPos_Y=%d arg3=%d widget->rect.y=%d height=%d\n",y,m_nBar_StartPos_Y,arg3,widget->rect.y,height);
                    //printf("[scroll bar] pos=%d bar->length=%d y=%d widget->rect.height=%d\n",pos,bar->length,y,widget->rect.height);

                    //만약 터치 영역이 bar의 내부라면 drag 플래그만 set하고
                    //누른 포지션은 보내지 않도록 한다.
                    if( y>=m_nBar_StartPos_Y && y<=(m_nBar_StartPos_Y+height) )
                    {
                        //bar 내부 터치
                        ituExecActions((ITUWidget*)bar, bar->actions, ITU_EVENT_CHANGED, 1000+pos);
                        printf("[scroll bar] bar toucn. dont action.\n");
                    }
                    else
                    {
                        //bar의 빈공간 터치
                        if (pos > bar->length)
                            pos = bar->length;

                        if (pos != bar->pos)
                        {
                            ituScrollBarSetPosition(bar, pos);
                            ituExecActions((ITUWidget*)bar, bar->actions, ITU_EVENT_CHANGED, pos);
                            ituScrollBarOnPositionChanged(bar, pos, true);
                        }
                    }
                }

				if (widget->flags & ITU_DRAGGABLE)
				{
					widget->flags |= ITU_DRAGGING;
					ituScene->dragged = widget;
				}

                result = widget->dirty = true;
            }
        }
    }
    else if (ev == ITU_EVENT_MOUSEUP)
    {
		if (widget->flags & ITU_DRAGGABLE)
		{
			widget->flags &= ~ITU_DRAGGING;

			if (ituScene->dragged == widget)
				ituScene->dragged = NULL;
		}
    }
    else if (ev == ITU_EVENT_MOUSEMOVE)
    {
        if (ituWidgetIsEnabled(widget) && (widget->flags & ITU_DRAGGING))
        {
            int x = arg2 - widget->rect.x;
            int y = arg3 - widget->rect.y;

            if (ituWidgetIsInside(widget, x, y))
            {
                if (bar->layout == ITU_LAYOUT_HORIZONTAL)
                {
                    int x = arg2 - widget->rect.x;
                    int pos = bar->length * x / widget->rect.width + 1;

                    if (pos > bar->length)
                        pos = bar->length;

                    if (pos != bar->pos)
                    {
                        ituScrollBarSetPosition(bar, pos);
                        ituExecActions((ITUWidget*)bar, bar->actions, ITU_EVENT_CHANGED, pos);
                        ituScrollBarOnPositionChanged(bar, pos, true);
                        result = widget->dirty = true;
                    }
                }
                else // if (bar->layout == ITU_LAYOUT_VERTICAL)
                {
                    //int y = arg3 - widget->rect.y;
                    //int pos = bar->length * y / widget->rect.height + 1;
                    //chpark.24.08.06
                    int height = (widget->rect.height/m_nTotalUser)*m_nPageCnt; //chpark.24.08.06
                    int y = arg3 - widget->rect.y;    
                    int pos = bar->length * y / (widget->rect.height - height) + 1;//한칸의 크기.   
                    
                    //printf("height=%d y=%d m_nBar_StartPos_Y=%d widget->rect.y=%d bar->length=%d widget->rect.height - height=%d pos=%d \n",height,y,m_nBar_StartPos_Y,widget->rect.y,bar->length,widget->rect.height - height,pos);

                    if (pos > bar->length)
                        pos = bar->length;

                    if (pos != bar->pos)
                    {
                        ituScrollBarSetPosition(bar, pos);
                        ituExecActions((ITUWidget*)bar, bar->actions, ITU_EVENT_CHANGED, pos);
                        ituScrollBarOnPositionChanged(bar, pos, true);
                        result = widget->dirty = true;
                    }
                }
            }
        }
    }
    result |= ituIconUpdate(widget, ev, arg1, arg2, arg3);

    return widget->visible ? result : false;
}

void ituScrollBarDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    uint8_t desta;
    ITURectangle prevClip;
    ITUScrollBar* bar = (ITUScrollBar*) widget;
    ITURectangle* rect = (ITURectangle*) &widget->rect;
    assert(bar);
    assert(dest);

    destx = rect->x + x;
    desty = rect->y + y;
    desta = alpha * widget->color.alpha / 255;
    desta = desta * widget->alpha / 255;

    ituWidgetSetClipping(widget, dest, x, y, &prevClip);

    if (desta == 255)//chpark.24.08.06
    //if (desta >= 0)//chpark.24.08.06
    {
        //printf("[ituScrollBarDraw][1] desta=%d m_nBar_StartPos_Y=%d \n",desta,m_nBar_StartPos_Y);
        ituColorFill(dest, destx, desty, rect->width, rect->height, &widget->color);
            
        if (bar->pos > 0 && bar->length > 0)
        {
			if (bar->layout == ITU_LAYOUT_HORIZONTAL)
			{
				double d_width = (double)(widget->rect.width) / bar->length;
				double c_width = ceil(d_width);
				double dev_real = c_width - d_width;
				double dev_stack = 0.0;
				int i, x = 0;
				int width;

				for (i = 1; i <= bar->pos; i++)
				{
					width = (int)c_width;

					if (i > 1)
						dev_stack += dev_real;

					if (dev_stack >= 1.0)
					{
						dev_stack -= 1.0;
						width--;
					}

					if (i == 1)
						x = 0;
					else
						x += width;
				}

				if (((x + width) > widget->rect.width) || (bar->pos == bar->length))
					ituColorFill(dest, destx + x, desty, (widget->rect.width - x), rect->height, &bar->fgColor);
				else
					ituColorFill(dest, destx + x, desty, width, rect->height, &bar->fgColor);

				//printf("[x %d] [width %d] [widget width %d]--[pos %d][length %d]\n", x, width, widget->rect.width, bar->pos, bar->length);
			}
			else //if (bar->layout == ITU_LAYOUT_VERTICAL)
			{
				//double d_height = (double)(widget->rect.height) / bar->length;
	
                //bar의 길이
                double d_height = (double)(widget->rect.height/m_nTotalUser)*m_nPageCnt; //chpark.24.08.06
                
                //double c_height = ceil(d_height); //chpark.24.08.06
                //페이지가 증가하거나 감소할 때 마다 실질적으로 이동하는 바의 움직임 길이 
                double c_height = ceil( (widget->rect.height-d_height)/(m_nTotalUser-m_nPageCnt)); //chpark.24.08.06

				double dev_real = c_height - d_height;
				double dev_stack = 0.0;
				
                int height = (int)d_height;
                int y = 0; // bar의 시작 위치
                y = (bar->pos - 1)*c_height;
                
                //y가 bar의 시작위치 이므로 y좌표를 바꿔야 한다.
				if (((y + height) > widget->rect.height) || (bar->pos == bar->length))
					ituColorFill(dest, destx, desty + y, rect->width, (widget->rect.height - y), &bar->fgColor);
				else
					ituColorFill(dest, destx, desty + y, rect->width, height, &bar->fgColor);

                m_nBar_StartPos_Y = y; //chpark.24.08.06
				//printf("[ituScrollBarDraw1][desta:%d][y %d] [height %d] [widget height %d]--[pos %d][length %d]\n", desta, y, height, widget->rect.height, bar->pos, bar->length);
                //printf("[ituScrollBarDraw1][pos %d][length %d]\n", bar->pos, bar->length);

            }
        }
    }
    else if (desta > 0)
    {
        //==================
        //chpark.24.08.06
        if (bar->pos > 0 && bar->length > 0)
        {
            //bar의 길이
            double d_height = (double)(widget->rect.height/m_nTotalUser)*m_nPageCnt; //chpark.24.08.06
                
            //double c_height = ceil(d_height); //chpark.24.08.06
            //페이지가 증가하거나 감소할 때 마다 실질적으로 이동하는 바의 움직임 길이 
            double c_height = ceil( (widget->rect.height-d_height)/(m_nTotalUser-m_nPageCnt)); //chpark.24.08.06
			
            int height = (int)d_height;
            int y = 0; // bar의 시작 위치
            y = (bar->pos - 1)*c_height;
            m_nBar_StartPos_Y = y;    

            printf("[ituScrollBarDraw][2] desta=%d m_nBar_StartPos_Y=%d \n",desta,m_nBar_StartPos_Y);
        }
        //============

        ITUSurface* surf = ituCreateSurface(rect->width, rect->height, 0, dest->format, NULL, 0);
        if (surf)
        {
            ituColorFill(surf, 0, 0, rect->width, rect->height, &widget->color);
            
            if (bar->pos > 0 && bar->length > 0)
            {
                if (bar->layout == ITU_LAYOUT_HORIZONTAL)
                {
                    int width = widget->rect.width / bar->length;
                    int x = (bar->pos - 1) * width;
                                
                    ituColorFill(surf, x, 0, width, rect->height, &bar->fgColor);
                }
                else //if (bar->layout == ITU_LAYOUT_VERTICAL)
                {
                    
                    //int height = widget->rect.height / bar->length;
                    int height = (widget->rect.height/m_nTotalUser)*m_nPageCnt; //chpark.24.08.06
                    //int y = (bar->pos - 1) * height;     
                    int y = (bar->pos - 1) * ((widget->rect.height - height)/(m_nTotalUser-m_nPageCnt+1));//한칸의 크기.    
                    
                    ituColorFill(surf, 0, y, rect->width, rect->height, &bar->fgColor);
                }
            }
            ituAlphaBlend(dest, destx, desty, rect->width, rect->height, surf, 0, 0, desta);                
            ituDestroySurface(surf);
        }
    }
    

    ituSurfaceSetClipping(dest, prevClip.x, prevClip.y, prevClip.width, prevClip.height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}

static void ScrollBarOnPositionChanged(ITUScrollBar* bar, int pos, bool confirm)
{
    // DO NOTHING
}

void ituScrollBarOnAction(ITUWidget* widget, ITUActionType action, char* param)
{
    ITUScrollBar* bar = (ITUScrollBar*) widget;
    assert(bar);

    switch (action)
    {
    case ITU_ACTION_RELOAD:
        if (param && param[0] != '\0')
        {
            char buf[ITU_ACTION_PARAM_SIZE], *saveptr;

            strcpy(buf, param);
            bar->pos = atoi(strtok_r(buf, " ", &saveptr));
            bar->length = atoi(strtok_r(NULL, " ", &saveptr));
            
            ituWidgetSetDirty(bar, true);
        }
        break;

    case ITU_ACTION_DODELAY0:
        ituExecActions(widget, bar->actions, ITU_EVENT_DELAY0, atoi(param));
        break;

    case ITU_ACTION_DODELAY1:
        ituExecActions(widget, bar->actions, ITU_EVENT_DELAY1, atoi(param));
        break;

    case ITU_ACTION_DODELAY2:
        ituExecActions(widget, bar->actions, ITU_EVENT_DELAY2, atoi(param));
        break;

    case ITU_ACTION_DODELAY3:
        ituExecActions(widget, bar->actions, ITU_EVENT_DELAY3, atoi(param));
        break;

    case ITU_ACTION_DODELAY4:
        ituExecActions(widget, bar->actions, ITU_EVENT_DELAY4, atoi(param));
        break;

    case ITU_ACTION_DODELAY5:
        ituExecActions(widget, bar->actions, ITU_EVENT_DELAY5, atoi(param));
        break;

    case ITU_ACTION_DODELAY6:
        ituExecActions(widget, bar->actions, ITU_EVENT_DELAY6, atoi(param));
        break;

    case ITU_ACTION_DODELAY7:
        ituExecActions(widget, bar->actions, ITU_EVENT_DELAY7, atoi(param));
        break;

    default:
        ituWidgetOnActionImpl(widget, action, param);
        break;
    }
}

void ituScrollBarInit(ITUScrollBar* bar)
{
    assert(bar);
    ITU_ASSERT_THREAD();

    memset(bar, 0, sizeof (ITUScrollBar));

    ituBackgroundInit(&bar->bg);
    ituWidgetSetType(bar, ITU_SCROLLBAR);
    ituWidgetSetName(bar, scrollBarName);
    ituWidgetSetUpdate(bar, ituScrollBarUpdate);
    ituWidgetSetDraw(bar, ituScrollBarDraw);
    ituWidgetSetOnAction(bar, ituScrollBarOnAction);
    ituScrollBarSetPositionChanged(bar, ScrollBarOnPositionChanged);
}

void ituScrollBarLoad(ITUScrollBar* bar, uint32_t base)
{
    assert(bar);

    ituBackgroundLoad(&bar->bg, base);
    ituWidgetSetUpdate(bar, ituScrollBarUpdate);
    ituWidgetSetDraw(bar, ituScrollBarDraw);
    ituWidgetSetOnAction(bar, ituScrollBarOnAction);
    ituScrollBarSetPositionChanged(bar, ScrollBarOnPositionChanged);
}

void ituScrollBarSetLength(ITUScrollBar* bar, int length)
{
    assert(bar);
    ITU_ASSERT_THREAD();

    if (length < 0)
    {
        LOG_WARN "incorrect length: %d\n", length LOG_END
        return;
    }
    bar->length = length;

    ituWidgetSetDirty(bar, true);
}

void ituScrollBarSetPosition(ITUScrollBar* bar, int pos)
{
    assert(bar);
    ITU_ASSERT_THREAD();

    if (pos < 0 || pos > bar->length)
    {
        LOG_WARN "incorrect pos: %d\n", pos LOG_END
        return;
    }
    bar->pos = pos;

    //printf("[ituScrollBarSetPosition] pos=%d\n",pos);

    ituWidgetSetDirty(bar, true);
}
