#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

#include "ite/itu.h"
#include "itu_cfg.h"
#include "itu_private.h"
#include "ite/itp.h"

static const char cbtName[] = "ITUComboTable";

#pragma warning (disable : 4090)

//use for tree config related
static bool m_bITU_Combo_MouseDn=false;
int nITU_Combo_SelectIndex;//chpark.01.03

static int nITU_Combo_PageItemCnt=0;//chpark.24.05.20
int nITU_Combo_nTop=0;//chpark.24.05.20

void ituComboTableDebugErr(const ITUWidget* widget, int error_line, int error_no)
{
    if (error_no)
    {
        char err_str[100] = "";

        switch (error_no)
        {
        case ITU_CBT_ERR_NOCHILD:
            strlcpy(err_str, "without child", sizeof(err_str));
            break;
        case ITU_CBT_ERR_CHILDNOTCOT:
            strlcpy(err_str, "first child is not container", sizeof(err_str));
            break;
        default:
            /* do nothing */
            break;
        }

        (void)printf("[CBT_Err][%s][%s][%s][%d]\n", widget->name, err_str, __FILE__, error_line);
    }
    else
    {
        (void)printf("[CBT_Err][%s][%s][%d]\n", widget->name, __FILE__, error_line);
    }
}

bool ituComboTableClone(ITUWidget* widget, ITUWidget** cloned)
{
    assert(widget);
    assert(cloned);
    ITU_ASSERT_THREAD();

    if (*cloned == NULL)
    {
        ITUComboTable* newObj = (ITUComboTable*)malloc(sizeof(ITUComboTable));
        ITUComboTable* oldObj = (ITUComboTable*)widget;
        ITUWidget* newWidget = (ITUWidget*)newObj;

        if (newWidget == NULL)
        {
            return false;
        }
        else
        {
            (void)memcpy(newObj, oldObj, sizeof(ITUComboTable));
            newWidget->tree.child = newWidget->tree.parent = newWidget->tree.sibling = NULL;
            *cloned = newWidget;
        }
    }

    return ituWidgetCloneImpl(widget, cloned);
}

ITUWidget* ComboTableGetVisibleChild(ITUComboTable* cbt)
{
    ITCTree *child, *tree = (ITCTree*)cbt;
    assert(tree);

    for (child = tree->child; child; child = child->sibling)
    {
        ITUWidget* widget = (ITUWidget*)child;

        if (widget->visible)
        {
            return widget;
        }
    }
    return NULL;
}
/*
void ituComboTableLoopExit(ITCTree* tree)
{
    if (tree != NULL)
    {
        ITCTree* item = tree;

        if (tree->child != NULL)
        {
            printf("[ituComboTableLoopExit][tree->child][1] delete.\n");
            ituComboTableLoopExit(tree->child);
            tree->child = NULL;
        }

        if (tree->sibling != NULL)
        {
            printf("[ituComboTableLoopExit][tree->sibling][2] delete.\n");
            ituComboTableLoopExit(tree->sibling);
            tree->sibling = NULL;
        }

        while (item != NULL)
        {
            printf("[ituComboTableLoopExit][tree][3] delete.\n");
            ITUWidget* widget = (ITUWidget*)item;
            ituWidgetExit(widget);
            break;
        }
    }
}
*/

//======================================================================
//set a global mutex for synchronization & adding null check.
//pthread_mutex_t treeMutex; //chpark.24.05.21

void ituComboTableLoopExit(ITCTree* tree)
{
	//======================
	//chpark.24.05.21 
    if (tree == NULL)
        return;
	//======================	

    //pthread_mutex_lock(&treeMutex);  // Lock for thread safety

    if (tree->child != NULL)
    {
        //printf("[ituComboTableLoopExit][tree->child][1] delete.\n");
        ituComboTableLoopExit(tree->child);
        tree->child = NULL;
    }

    if (tree->sibling != NULL)
    {
        //printf("[ituComboTableLoopExit][tree->sibling][2] delete.\n");
        ituComboTableLoopExit(tree->sibling);
        tree->sibling = NULL;
    }

    if (tree != NULL)
    {
        //printf("[ituComboTableLoopExit][tree][3] delete.\n");
        ITUWidget* widget = (ITUWidget*)tree;
        if (widget != NULL) // Validate widget before use
        {
            ituWidgetExit(widget);
        }
    }

    //pthread_mutex_unlock(&treeMutex); // Unlock after operations
}
//======================================================================

void ituComboTableExit(ITUWidget* widget)
{
    ITUComboTable* cbt = (ITUComboTable*)widget;
    assert(widget);
    ITU_ASSERT_THREAD();

    if (cbt->surf != NULL)
    {
        ituSurfaceRelease(cbt->surf);
        cbt->surf = NULL;
    }

    if (cbt->itemSurf != NULL)
    {
        ituSurfaceRelease(cbt->itemSurf);
        cbt->itemSurf = NULL;
    }

    (void)printf("\n[[DEBUG] HeapSize: %ld]\n", itpGetFreeHeapSize());
    if (cbt->itemcount > 0)
    {
        const ITCTree* tree = (ITCTree*)cbt;
        ITCTree* child = tree->child;
        ituComboTableLoopExit(child);
    }
    (void)printf("[[DEBUG] HeapSize: %ld ,  ggCount %d, exitCount %d]\n", itpGetFreeHeapSize(), ggCount, eeCount);
    (void)printf("\n");

	//pthread_mutex_destroy(&treeMutex);//chpark.24.05.21
}

//use refParent to find the cloned parent
ITUWidget* ituComboTableFindTargetParent(ITUWidget* cbt_widget, ITUWidget* refParent)
{
    ITUComboTable* cbt = (ITUComboTable*)cbt_widget;
    assert(cbt);
    if (cbt->tmpObj)
    {
        const ITCTree* tree = (ITCTree*)cbt->tmpObj;
        ITCTree* node = tree->child;
        char str[100] = "";
        snprintf(str, sizeof(str), "%X_%d", (int)&(*refParent), cbt->itemcount - 1);

        if (strcmp(cbt->tmpObj->name, str) == 0)
        {
            return cbt->tmpObj;
        }

        for (; node; node = node->sibling)
        {
            ITUWidget * checkWidget = (ITUWidget *)node;
            if (strcmp(checkWidget->name, str) == 0)
            {
                return checkWidget;
            }

            if (node->child)
            {
                ituComboTableFindTargetParent(checkWidget, refParent);
            }
        }
    }

    return NULL;
}

//chpark.24.05.21
void ituComboTable_SetPageItemCount(int nCount)
{
	nITU_Combo_PageItemCnt = nCount;
	//printf("[ituComboTable_SetPageItemCount]=%d\n",ituComboTable_SetPageItemCount);
}

bool ituComboTableUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    ITUComboTable* cbt = (ITUComboTable*)widget;
    assert(cbt);

    if (ev == ITU_EVENT_LOAD)
    {
        ituComboTableLoadStaticData(cbt);
    }
    else if (ev == ITU_EVENT_LAYOUT)
    {
        ITCTree *tree = (ITCTree*)cbt;
        ITUWidget* first_child_widget = (ITUWidget*)(tree->child);
        int error_num = -1;
        
        //debug tick
        ggCount = 0;
        eeCount = 0;

        //check setup rule
        if (first_child_widget)
        {
            if (first_child_widget->type != ITU_CONTAINER)
            {
                error_num = ITU_CBT_ERR_CHILDNOTCOT;
            }
            else
            {
                cbt->itemArr[0] = first_child_widget;
            }
        }
        else
        {
            error_num = ITU_CBT_ERR_NOCHILD;
        }

        if (error_num >= 0)
        {
            ituComboTableDebugErr(widget, __LINE__, error_num);
            return false;
        }
        else
        {
            int i = 0;
            int j = cbt->itemcount;
            int p = 0;
            int count = 0;
            cbt->itemcount = 0;
            //check current valid item count
            for (i = 0; i < ITU_WIDGET_CHILD_MAX; i++)
            {
                if (cbt->itemArr[i] != NULL)
                {
                    cbt->itemcount++;
                }
                else
                {
                    break;
                }
            }
            //grow item upto the target item count
            for (i = cbt->itemcount; i < j; i++)
            {
                if (cbt->itemArr[i] == NULL)
                {
                    ituComboTableAddItem(widget, widget);
                }
            }
            //rearrange item position
            p = cbt->currPage * cbt->pagemaxitem;
            for (i = p; i < ITU_WIDGET_CHILD_MAX; i++)
            {
                if (cbt->itemArr[i] != NULL)
                {
                    count++;
                    if (i == p)
                    {
                        continue;
                    }
                    else
                    {
                        int yPos = (cbt->itemArr[i - 1]->rect.y + cbt->itemArr[i - 1]->rect.height);
                        ituWidgetSetPosition(cbt->itemArr[i], cbt->itemArr[i]->rect.x, yPos);
                    }

                    if (count >= cbt->pagemaxitem)
                    {
                        count = 0;
                        p     = i + 1;
                        if (p < ITU_WIDGET_CHILD_MAX)
                        {
                            if (cbt->itemArr[p])
                            {
                                ituWidgetSetPosition(cbt->itemArr[p], cbt->itemArr[p]->rect.x, 0);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }
        cbt->hv_mode = 0;
        widget->dirty = 1;
    }
    else if (ev == ITU_EVENT_MOUSEDOWN)
    {
        if (widget->flags & ITU_DRAGGABLE)
        {
            int x = arg2 - widget->rect.x;
            int y = arg3 - widget->rect.y;
            if (ituWidgetIsInside(widget, x, y))
            {
                //chpark.01.03
				/*
				if (ituScene->dragged == NULL)
				{
					cbt->initX = cbt->touchX = x;
					cbt->initY = cbt->touchY = y;
					widget->flags |= ITU_DRAGGING;
					ituScene->dragged = widget;
				}

				cbt->lastclock = itpGetTickCount();
				*/
				//chpark.01.03
                //chpark.01.03
			    int nDN_Index=0;
				if (ituScene->dragged == NULL)
				{
					cbt->initX = cbt->touchX = x;
					cbt->initY = cbt->touchY = y;
					widget->flags |= ITU_DRAGGING;
					ituScene->dragged = widget;

					int i = 0;
					
					//use currpage to calculate start index of current page
					int p = cbt->currPage * cbt->pagemaxitem;
					for (i = 0; i < cbt->pagemaxitem; i++)
					{
						if (cbt->itemArr[p])
						{
							ITUWidget* item = cbt->itemArr[p];
							//inside check must consider with target rect.x/y
							if (ituWidgetIsInside(item, x - item->rect.x, y - item->rect.y))
							{
								if (cbt->selectIndex >= 0)
									cbt->lastselectIndex = cbt->selectIndex;
								cbt->selectIndex = p;
								
								ituExecActions((ITUWidget*)cbt, cbt->actions, ITU_EVENT_MOUSEUP, cbt->selectIndex);
								printf("[ITU_COMBO] ITU_EVENT_MOUSEDOWN(relpace UP):Index11:%d\n",cbt->selectIndex);
								break;
							}
						}
						p++;
					}
				}

				cbt->lastclock = itpGetTickCount();
				m_bITU_Combo_MouseDn = true;
				//===========================
            }
            else
			{
				printf("[ITU_COMBO] ITU_EVENT_MOUSEDOWN[2]\n");
				//m_bITU_Combo_MouseDn = false;//chpark.01.03
			}
        }//if (widget->flags & ITU_DRAGGABLE)
    }
    else if (ev == ITU_EVENT_MOUSEUP)
    {
        if (widget->flags & ITU_DRAGGING)
		{
			cbt->touchX = 0;
			cbt->touchY = 0;
			widget->flags &= ~ITU_DRAGGING;
			ituScene->dragged = NULL;

			cbt->clock = itpGetTickCount();
			if ((cbt->clock - cbt->lastclock) <= ITU_COMBOTABLE_MIT)
			{
				int x = arg2 - widget->rect.x;
				int y = arg3 - widget->rect.y;
				if (ituWidgetIsInside(widget, x, y))
				{
					int i = 0;
					
					//use currpage to calculate start index of current page
					int p = cbt->currPage * cbt->pagemaxitem;
					for (i = 0; i < cbt->pagemaxitem; i++)
					{
						if (cbt->itemArr[p])
						{
							ITUWidget* item = cbt->itemArr[p];
							//inside check must consider with target rect.x/y
							if (ituWidgetIsInside(item, x - item->rect.x, y - item->rect.y))
							{
								int nTouchPosX = x - item->rect.x;
								if(nTouchPosX<(item->rect.width/2))
								{
									//left
									nITU_Combo_SelectIndex = cbt->selectIndex*10;			
								}
								else
								{
									//right
									nITU_Combo_SelectIndex = cbt->selectIndex*10+1;			
								}


								if(cbt->selectIndex==p)//chpark.01.03
								{
									//if (cbt->selectIndex >= 0)
									//	cbt->lastselectIndex = cbt->selectIndex;
									//cbt->selectIndex = p;
									//printf("[CBT]select index %d\n", p);
									//printf("[ITU_COMBO] ITU_EVENT_SELECT[1]===nSelectIndex:%d  m_bITU_Combo_MouseDn=%d\n",nITU_Combo_SelectIndex,m_bITU_Combo_MouseDn);

									//if(m_bITU_Combo_MouseDn)//chpark.01.03
									{
										m_bITU_Combo_MouseDn = false;
										
										ituExecActions((ITUWidget*)cbt, cbt->actions, ITU_EVENT_SELECT, nITU_Combo_SelectIndex);
										//printf("[ITU_COMBO] ITU_EVENT_SELECT[2]===nSelectIndex:%d\n",nITU_Combo_SelectIndex);
									}
									
								}
								else
								{
									//printf("[ITU_COMBO] ITU_EVENT_SELECT Error[4]:%d %d ===\n",cbt->selectIndex,p);
								}
								break;
							}
						}
						p++;
					}
					
				}

				//send real mouse up event.
				//ituExecActions((ITUWidget*)cbt, cbt->actions, ITU_EVENT_CUSTOM+12, 0);
				//printf("[ITU_COMBO] ITU_EVENT_SELECT Error[5]:%d m_bITU_Combo_MouseDn:%d===\n",cbt->selectIndex,m_bITU_Combo_MouseDn);
				m_bITU_Combo_MouseDn = false;
				//printf("[ITU_COMBO] ITU_EVENT_MOUSEUP(ITU_EVENT_CUSTOM+12)===\n");
				//
			}
			
			//In combo box touch-up, the event below is sent to accurately match the line.
			//*Originally, I tried to receive the event by passing the parameter value in touchup, but
			//There was a problem in the last parameter where the integer value was not transmitted.
			//It was modified as follows.
			//printf("[ITU_COMBO] ITU_EVENT_SELECT_NEW->Reposition[5]:%d\n",m_bITU_Combo_MouseDn);
			ituExecActions(widget, cbt->actions, ITU_EVENT_CHANGED, 1000);
		}
		else
		{
			//other
			//If there are two controls, this is an event for the disabled control.
		}
    }
    else if (ev == ITU_EVENT_MOUSEMOVE)
    {
       //===================================
		//chpark.24.05.20
		if (widget->flags & ITU_DRAGGING)
		{
			int x = arg2 - widget->rect.x;
			int y = arg3 - widget->rect.y;
			int distX = x - cbt->touchX;
			int distY = y - cbt->touchY;
			int Xtd = x - cbt->initX;
			int Ytd = y - cbt->initY;
			bool X_dir = false;//(abs(distX) > abs(distY)) ? (true) : (false);
			bool Y_dir = (abs(distX) < abs(distY)) ? (true) : (false);

			bool bY_Move = true;//chpark.24.05.20
			nITU_Combo_nTop =  (-1)*(cbt->itemArr[0]->rect.height)*(cbt->itemcount-nITU_Combo_PageItemCnt);

			if(cbt->itemcount<=nITU_Combo_PageItemCnt)
			{
				bY_Move = false;
			}
			else 
			{
				//nTop = (-1)*(cbt->itemArr[0]->rect.height)*(cbt->itemcount-7);
				
				if(cbt->itemArr[0]->rect.y<=nITU_Combo_nTop)
				{
					//printf("[ITU][Don't move]==nITU_Combo_PageItemCnt=%d\n",nITU_Combo_PageItemCnt);
					bY_Move = false;
				}

				if(distY>0)
				{
					bY_Move = true;
				}
			}

			if ((abs(Xtd) >= ITU_DRAG_DISTANCE) || (abs(Ytd) >= ITU_DRAG_DISTANCE))
			{
				ituUnPressWidget(widget);
			}

			if (ituWidgetIsInside(widget, x, y))
			{
				if(bY_Move)
				{
					ituComboTableMoveXY(cbt, distX, distY);//chpark.24.05.20
				}
				cbt->touchX = x;
				cbt->touchY = y;
				widget->dirty = 1;
			}
			//===================================
		}//if (widget->flags & ITU_DRAGGING)
    }//if (ev == ITU_EVENT_MOUSEMOVE)
    else if (((ev == ITU_EVENT_TOUCHSLIDEUP) || (ev == ITU_EVENT_TOUCHSLIDEDOWN)) && (cbt->hv_mode == 0) && (cbt->sliding == 1))
    {
        if (ev == ITU_EVENT_TOUCHSLIDEUP)
        {
            cbt->inc = -1;
        }
        else
        {
            cbt->inc = 1;
        }
        cbt->comboTableFlags |= ITU_COMBOTABLE_SLIDING;
    }
    else if (ev == ITU_EVENT_TIMER)
    {
        if ((cbt->comboTableFlags & ITU_COMBOTABLE_SLIDING) && (!(widget->flags & ITU_DRAGGING)))
        {
            if (cbt->itemArr[0])
            {
                //chpark.24.06.27
                if (cbt->frame < cbt->totalframe)
                {
                    int distX = 0;
                    int distY = 0;
                    int itemBaseSize = (cbt->hv_mode == 0) ? (cbt->itemArr[0]->rect.height) : (cbt->itemArr[0]->rect.width);
                    float stepDist = itemBaseSize * 1.0; //modify here to change the slide distance
                    stepDist = cbt->inc * stepDist * exp((cbt->frame * -5.0 / cbt->totalframe));
                    distX = (cbt->hv_mode == 0) ? (0) : (stepDist);
                    distY = (cbt->hv_mode == 0) ? (stepDist) : (0);
                    ituComboTableMoveXY(cbt, distX, distY);
                    cbt->frame++;
                    widget->dirty = 1;
                }
                else
                {
                    cbt->comboTableFlags &= ~ITU_COMBOTABLE_SLIDING;
                    cbt->inc = 0;
                    cbt->frame = 0;
                     //chpark.24.06.27
                    //printf("[ITU][Timer][slide finished][1]==nITU_Combo_PageItemCnt=%d\n",nITU_Combo_PageItemCnt);
                    ituExecActions(widget, cbt->actions, ITU_EVENT_CHANGED, 2000);
                }
                
            }
            else
            {
                cbt->comboTableFlags &= ~ITU_COMBOTABLE_SLIDING;
                cbt->inc = 0;
                //printf("[ITU][Timer][slide finished][1]==nITU_Combo_PageItemCnt=%d\n",nITU_Combo_PageItemCnt);
                ituExecActions(widget, cbt->actions, ITU_EVENT_CHANGED, 2000);
            }
        }
    }
    else
    {
        /* do nothing */
    }

    result |= widget->dirty;
    result |= ituWidgetUpdateImpl(widget, ev, arg1, arg2, arg3);

    return widget->visible ? result : false;
}

void ituComboTableDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITUComboTable* cbt = (ITUComboTable*)widget;
    ITURectangle* rect = &widget->rect;
    ITURectangle prevClip;
    assert(cbt);
    assert(dest);

    if (!widget->visible)
        return;

    destx = rect->x + x;
    desty = rect->y + y;
    //desta = alpha * widget->alpha / 255; //useless now

    //when item count is 0, draw nothing
    if (cbt->itemcount == 0)
    {
        return;
    }

    ituWidgetSetClipping(widget, dest, x, y, &prevClip);

    //draw background
    if (cbt->surf)
    {
        if (widget->flags & ITU_STRETCH)
        {
#ifndef CFG_M2D_ENABLE
            if (widget->angle == 0)
            {
                ituStretchBlt(dest, destx, desty, rect->width, rect->height, cbt->surf, 0, 0, cbt->surf->width, cbt->surf->height);
            }
            else
            {
                float scaleX, scaleY;
                if (widget->angle == 90 || widget->angle == 270)
                {
                    scaleX = (float)rect->width / cbt->surf->height;
                    scaleY = (float)rect->height / cbt->surf->width;
                }
                else
                {
                    scaleX = (float)rect->width / cbt->surf->width;
                    scaleY = (float)rect->height / cbt->surf->height;
                }
                ituRotate(dest, destx + rect->width / 2, desty + rect->height / 2, cbt->surf, cbt->surf->width / 2, cbt->surf->height / 2, (float)widget->angle, scaleX, scaleY);
            }
#else
            int desta = alpha * widget->alpha / 255;
            float scaleX = (float)rect->width / cbt->surf->width;
            float scaleY = (float)rect->height / cbt->surf->height;

            ituTransform(
                dest, destx, desty, rect->width, rect->height,
                cbt->surf, 0, 0, cbt->surf->width, cbt->surf->height,
                cbt->surf->width / 2, cbt->surf->height / 2,
                scaleX,
                scaleY,
                (float)widget->angle,
                ITU_TILE_FILL,
                true,
                true,
                desta);
#endif
        }
        else
        {
            if (widget->angle == 0)
            {
                ituBitBlt(dest, destx, desty, rect->width, rect->height, cbt->surf, 0, 0);
            }
            else
            {
#ifndef CFG_M2D_ENABLE
                ituRotate(dest, destx + rect->width / 2, desty + rect->height / 2, cbt->surf, cbt->surf->width / 2, cbt->surf->height / 2, (float)widget->angle, 1.0f, 1.0f);
#else
                ituRotate(dest, destx, desty, cbt->surf, cbt->surf->width / 2, cbt->surf->height / 2, (float)widget->angle, 1.0f, 1.0f);
#endif
            }
        }
    }
    else
    {
        ituColorFill(dest, destx, desty, rect->width, rect->height, &widget->color);
    }

    //draw item background
    if (cbt->itemSurf)
    {
        int drawcount = cbt->itemcount - (cbt->currPage * cbt->pagemaxitem);
        if ((drawcount > 0) && cbt->itemArr[0])
        {
            int i = 0;
            for (i = 0; i < drawcount; i++)
            {
                int cid = (cbt->currPage * cbt->pagemaxitem) + i;

                //========================================
				//chpark.24.03.13
				if(cbt->itemArr[cid]->rect.y<-20)
				{
					//printf("[ituComboTableDraw][Not Draw]cid=%d drawcount=%d itemy=%d desty=%d rect.y=%d widget->rect.height=%d\n",cid,drawcount,itemy,desty,cbt->itemArr[cid]->rect.y,widget->rect.height);//chpark.24.03.13
					continue;
				}
				else
				{
					//printf("[ituComboTableDraw][Draw]cid=%d drawcount=%d desty=%d rect.y=%d \n",cid,drawcount,desty,cbt->itemArr[cid]->rect.y);//chpark.24.03.13
				}
				//========================================

                //int itemx = destx;
                //int itemy = desty + (i * cbt->itemArr[0]->rect.height);
                int itemx = destx + cbt->itemArr[cid]->rect.x;
                int itemy = desty + cbt->itemArr[cid]->rect.y;

                //if ((itemy + cbt->itemSurf->height) > (desty + widget->rect.height))
                //  break;
                if (itemy > (desty + widget->rect.height))
                {
                    break;
                }

                if (widget->flags & ITU_STRETCH)
                {
#ifndef CFG_M2D_ENABLE
                    if (widget->angle == 0)
                    {
                        ituAlphaBlend(dest, itemx, itemy, cbt->itemSurf->width, cbt->itemSurf->height, cbt->itemSurf, 0, 0, 255);
                    }
                    else
                    {
                        float scaleX, scaleY;
                        if (widget->angle == 90 || widget->angle == 270)
                        {
                            scaleX = (float)cbt->itemArr[0]->rect.width / cbt->itemSurf->height;
                            scaleY = (float)cbt->itemArr[0]->rect.height / cbt->itemSurf->width;
                        }
                        else
                        {
                            scaleX = (float)cbt->itemArr[0]->rect.width / cbt->itemSurf->width;
                            scaleY = (float)cbt->itemArr[0]->rect.height / cbt->itemSurf->height;
                        }
                        ituRotate(dest, itemx + cbt->itemArr[0]->rect.width / 2, itemy + cbt->itemArr[0]->rect.height / 2, cbt->itemSurf, cbt->itemSurf->width / 2, cbt->itemSurf->height / 2, (float)widget->angle, scaleX, scaleY);
                    }
#else
                    int desta = alpha * widget->alpha / 255;
                    float scaleX = (float)cbt->itemArr[0]->rect.width / cbt->itemSurf->width;
                    float scaleY = (float)cbt->itemArr[0]->rect.height / cbt->itemSurf->height;

                    ituTransform(
                        dest, itemx, itemy, cbt->itemArr[0]->rect.width, cbt->itemArr[0]->rect.height,
                        cbt->itemSurf, 0, 0, cbt->itemSurf->width, cbt->itemSurf->height,
                        cbt->itemSurf->width / 2, cbt->itemSurf->height / 2,
                        scaleX,
                        scaleY,
                        (float)widget->angle,
                        ITU_TILE_FILL,
                        true,
                        true,
                        desta);
#endif
                }
            }
        }
    }
    ituSurfaceSetClipping(dest, prevClip.x, prevClip.y, prevClip.width, prevClip.height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}

void ituComboTableOnAction(ITUWidget* widget, ITUActionType action, char* param)
{
    ITUComboTable* cbt = (ITUComboTable*)widget;
    assert(cbt);

    switch (action)
    {
    case ITU_ACTION_NEXT:
        {
            int vCurrPage = ituComboTableGetCurrentPageNumber(cbt);
            if (vCurrPage >= 0)
            {
                int vTargetPage = vCurrPage + 1;
                ituComboTableGotoPage(cbt, vTargetPage);
            }
        }
        break;
    case ITU_ACTION_PREV:
        {
            int vCurrPage = ituComboTableGetCurrentPageNumber(cbt);
            if (vCurrPage >= 0)
            {
                int vTargetPage = vCurrPage - 1;
                ituComboTableGotoPage(cbt, vTargetPage);
            }
        }
        break;
    default:
        ituWidgetOnActionImpl(widget, action, param);
        break;
    }
}

void ituComboTableInit(ITUComboTable* cbt)
{
    assert(cbt);
    ITU_ASSERT_THREAD();

    (void)memset(cbt, 0, sizeof (ITUComboTable));

    ituWidgetInit(&cbt->widget);

    ituWidgetSetType(cbt, ITU_COMBOTABLE);
    ituWidgetSetName(cbt, cbtName);
    ituWidgetSetExit(cbt, ituComboTableExit);
    ituWidgetSetUpdate(cbt, ituComboTableUpdate);
    ituWidgetSetDraw(cbt, ituComboTableDraw);
    ituWidgetSetOnAction(cbt, ituComboTableOnAction);
    ituWidgetSetClone(cbt, ituComboTableClone);

	//pthread_mutex_init(&treeMutex, NULL);//chpark.24.05.21
}

void ituComboTableLoad(ITUComboTable* cbt, uint32_t base)
{
    assert(cbt);

    ituWidgetLoad(&cbt->widget, base);
    ituWidgetSetExit(cbt, ituComboTableExit);
    ituWidgetSetUpdate(cbt, ituComboTableUpdate);
    ituWidgetSetDraw(cbt, ituComboTableDraw);
    ituWidgetSetOnAction(cbt, ituComboTableOnAction);
    ituWidgetSetClone(cbt, ituComboTableClone);

    //load the background image
    if (cbt->staticSurf)
    {
        ITUSurface* surf = (ITUSurface*)(base + (uint32_t)cbt->staticSurf);
        if (surf->flags & ITU_COMPRESSED)
        {
            cbt->surf = NULL;
        }
        else
        {
            cbt->surf = ituCreateSurface(surf->width, surf->height, surf->pitch, surf->format, (const uint8_t *)surf->addr,
                                         surf->flags);
        }

        cbt->staticSurf = surf;
    }

    //load the item background image
    if (cbt->itemStaticSurf)
    {
        ITUSurface* surf = (ITUSurface*)(base + (uint32_t)cbt->itemStaticSurf);
        if (surf->flags & ITU_COMPRESSED)
        {
            cbt->itemSurf = NULL;
        }
        else
        {
            cbt->itemSurf = ituCreateSurface(surf->width, surf->height, surf->pitch, surf->format,
                                             (const uint8_t *)surf->addr, surf->flags);
        }

        cbt->itemStaticSurf = surf;
    }
}

void ituComboTableAddItem(ITUWidget* cbtWidget, ITUWidget* widget)
{
    ITUComboTable* cbt = (ITUComboTable*)cbtWidget;
    assert(cbt);

    if (cbt->itemcount == 0)
    {
        cbt->itemcount++;
        return;
    }
    else
    {
        const ITCTree *tree = (ITCTree*)widget;
        ITCTree* child = tree->child;
        for (; child; child = child->sibling)
        {
            ITUWidget* obj = (ITUWidget*)child;
            ITUWidget* cloned = NULL;

            if (ituWidgetClone(obj, &cloned))//ggyy
            {
                ITCTree* itc_Cloned = (ITCTree*)cloned;
                char str[100] = "";
                //update itemcount here before make name str
                if ((cloned->type == ITU_CONTAINER) && (widget->type == ITU_COMBOTABLE))
                {
                    cbt->itemArr[cbt->itemcount] = cloned;
                    cbt->itemcount++;
                }
                //here for debug
                //if (cbt->itemcount <= 3)
                //  (void)printf("[CBT][%s] clone [child %s] [parent %s]\n", cbtWidget->name, cloned->name, widget->name);

                //make name str
                snprintf(str, sizeof(str), "%X_%d", (int)&(*obj), cbt->itemcount - 1);
                ituWidgetSetName(cloned, str);


                //clear the tree of cloned obj
                if (obj->type != ITU_BUTTON)
                {
                    itc_Cloned->parent = NULL;
                    itc_Cloned->child = NULL;
                    itc_Cloned->sibling = NULL;
                }

                if ((cloned->type == ITU_CONTAINER) && (widget->type == ITU_COMBOTABLE))
                {
                    cbt->tmpObj = cloned;
                    ituWidgetAdd(widget, cloned);

                    if (cbt->itemcount >= 2)
                    {
                        if (cbt->itemcount <= cbt->pagemaxitem) //only one page
                        {
                            int vRefItemIdx = cbt->itemcount - 2;
                            ituWidgetSetY(cloned, ituWidgetGetY(cbt->itemArr[vRefItemIdx]) + ituWidgetGetHeight(cbt->itemArr[vRefItemIdx]));
                        }
                        else
                        {
                            if ((cbt->itemcount % cbt->pagemaxitem) == 1)
                            {
                                ituWidgetSetY(cloned, 0);
                            }
                            else
                            {
                                int vRefItemIdx = cbt->itemcount - 2;
                                ituWidgetSetY(cloned, ituWidgetGetY(cbt->itemArr[vRefItemIdx]) + ituWidgetGetHeight(cbt->itemArr[vRefItemIdx]));
                            }
                            // hide the item that not located in current page
                            if (ituComboTableGetMaxPageIndex(cbt) != cbt->currPage)
                            {
                                ituWidgetSetVisible(cloned, false);
                            }
                            else
                            {
                                ituWidgetSetVisible(cloned, true);
                            }
                        }

                    }
                }
                else
                {
                    ITUWidget * refParent = ituComboTableFindTargetParent(cbtWidget, widget);
                    if (refParent)
                    {
                        ituWidgetAdd(refParent, cloned);
                    }
                }

                if (child->child)
                {
                    ITUWidget* source = (ITUWidget*)child;
                    const ITUWidget* target = (ITUWidget*)child->child;
                    if (strlen(target->name))
                    {
                        ituComboTableAddItem(cbtWidget, source);
                    }
                }

                if ((cloned->type == ITU_CONTAINER) && (widget->type == ITU_COMBOTABLE))
                {
                    return;
                }
            }
        }//////
    }
}

bool ituComboTableAdd(ITUComboTable* cbtWidget, int count)
{
    if (cbtWidget)
    {
        if (count > 0)
        {
            if ((cbtWidget->itemcount + count) > ITU_COMBOTABLE_MAX_ITEM_COUNT)
            {
                (void)printf("[CBT]can not add %d item! current item: %d / max %d\n", count, cbtWidget->itemcount,
                       ITU_COMBOTABLE_MAX_ITEM_COUNT);
            }
            else
            {
                cbtWidget->itemcount += count;
                //ituWidgetUpdate(cbtWidget, ITU_EVENT_LAYOUT, 0, 0, 0);//chpark.24.03.11
                return true;
            }
        }
    }
    return false;
}

void ituComboTableLoadStaticData(ITUComboTable* cbt)
{
    const ITUWidget* widget = (ITUWidget*)cbt;
    ITUSurface* surf = NULL;
    ITUSurface* isurf = NULL;

    if (widget->flags & ITU_EXTERNAL)
    {
        return;
    }

    if (cbt->staticSurf && !cbt->surf)
    {
        surf = cbt->staticSurf;

        if (surf->flags & ITU_COMPRESSED)
        {
            cbt->surf = ituSurfaceDecompress(surf);
        }
        else
        {
            cbt->surf = ituCreateSurface(surf->width, surf->height, surf->pitch, surf->format,
                                         (const uint8_t *)surf->addr, surf->flags);
        }
    }

    if (cbt->itemStaticSurf && !cbt->itemSurf)
    {
        isurf = cbt->itemStaticSurf;

        if (isurf->flags & ITU_COMPRESSED)
        {
            cbt->itemSurf = ituSurfaceDecompress(isurf);
        }
        else
        {
            cbt->itemSurf = ituCreateSurface(isurf->width, isurf->height, isurf->pitch, isurf->format,
                                             (const uint8_t *)isurf->addr, isurf->flags);
        }
    }
}

void ituComboTableMoveXY(ITUComboTable* cbt, int distX, int distY)
{
    if (cbt->itemArr[0])
    {
        int modY = 0;
        int vPageFirstIndex = cbt->currPage * cbt->pagemaxitem;
        int vPageLastIndex = (cbt->currPage < ituComboTableGetMaxPageIndex(cbt)) ?
            (vPageFirstIndex + cbt->pagemaxitem - 1) :
            ((cbt->currPage * cbt->pagemaxitem) + (cbt->itemcount % cbt->pagemaxitem) - 1);

        ITUWidget* pPageFirstItem = cbt->itemArr[vPageFirstIndex];
        ITUWidget* pPageLastItem = cbt->itemArr[vPageLastIndex];

        if (distY <= 0)
        {
            modY = ((pPageLastItem->rect.y + distY) < 0) ? (0 - pPageLastItem->rect.y) : (distY);
        }
        else
        {
            modY = ((pPageFirstItem->rect.y + distY) > 0) ? (0 - pPageFirstItem->rect.y) : (distY);
        }

        if (modY != 0)
        {
            int i = vPageFirstIndex;
            while (cbt->itemArr[i] != NULL)
            {
                cbt->itemArr[i]->rect.y += modY;
                i++;
                if (i > vPageLastIndex)
                {
                    break;
                }
            }
        }

        if (distX != 0)
        {
            //(void)printf("\n");
        }
    }
}


int ituComboTableGetMaxPageIndex(ITUComboTable* cbt)
{
    assert(cbt);
    if (cbt->itemcount == 0)
    {
        return -1;
    }
    else
    {
        int vMaxPageIdx = (cbt->itemcount / cbt->pagemaxitem);
        if (vMaxPageIdx && (vMaxPageIdx > cbt->pagemaxitem))
        {
            vMaxPageIdx++;
        }
        return vMaxPageIdx;
    }
}

//goto other page (goto current page will reset item position only)
void ituComboTableGotoPage(ITUComboTable* cbt, int page)
{
    assert(cbt);
    if (page < 0)
    {
        return;
    }
    else if (page > ituComboTableGetMaxPageIndex(cbt))
    {
        return;
    }
    else
    {
        int i = cbt->currPage * cbt->pagemaxitem;
        int j = page * cbt->pagemaxitem;
        int x = 0;
        bool vSamePage = (page == cbt->currPage);
        for (; x < cbt->pagemaxitem; x++)
        {
            if (cbt->itemArr[i])
            {
                ituWidgetSetVisible(cbt->itemArr[i], vSamePage);

                if (x == 0)
                {
                    ituWidgetSetY(cbt->itemArr[i], 0);
                }
                else
                {
                    ituWidgetSetY(cbt->itemArr[i], cbt->itemArr[i - 1]->rect.y + cbt->itemArr[i - 1]->rect.height);
                }
            }

            if ((cbt->itemArr[j]) && (!vSamePage))
            {
                ituWidgetSetVisible(cbt->itemArr[j], true);
            }

            i++;
            j++;
        }
        if (cbt->currPage != page)
        {
            cbt->currPage = page;
            ituExecActions((ITUWidget*)cbt, cbt->actions, ITU_EVENT_CHANGED, cbt->currPage);
        }
    }
}


void ituComboTableChildArray(const ITCTree* tree, ITUWidget** widget_arr)
{
    if (tree)
    {
        ITCTree* child = tree->child;
        for (; child; child = child->sibling)
        {
            ITUWidget * widget = (ITUWidget *)child;
            *widget_arr        = widget;
            widget_arr++;

            if (child->child)
            {
                ituComboTableChildArray(child, widget_arr);
            }
        }
    }
}


ITUWidget* ituComboTableGetItemChild(ITUComboTable* cbt, int item_index, ITUWidgetType type, int child_index)
{
    assert(cbt);
    if ((item_index < 0) || (item_index >= cbt->itemcount) || (child_index < 0))
    {
        return NULL;
    }
    else
    {
        const ITCTree* tree = (ITCTree*)cbt->itemArr[item_index];
        if (tree)
        {
            int i = 0;
            int target_index = -1;
            ITUWidget* widgetArr[128] = { NULL };

            if (child_index >= 128)
            {
                return NULL;
            }

            ituComboTableChildArray(tree, &widgetArr[0]);

            for (i = 0; i < 128; i++)
            {
                ITUWidget* child = widgetArr[i];
                if (child)
                {
                    if (child->type == type)
                    {
                        target_index++;
                    }
                    if (target_index == child_index)
                    {
                        return child;
                    }
                }
                else
                {
                    break;
                }
            }

            return NULL;
        }
        else
        {
            return NULL;
        }
    }
}

void ituComboTableDelItem(ITUComboTable* cbt, int item_index)
{
    if (cbt)
    {
        if (item_index == 0)
        {
            return;
        }

        if (cbt->itemArr[0])
        {
			//=======================================================
			//chpark.24.05.21

			/*
            const ITCTree * tree   = (ITCTree *)cbt;
            ITCTree *       child  = tree->child;
            int i = 1;  //0 will return
            bool vFound = false;
            for (; child; child = child->sibling)
            {
                ITUWidget* obj = (ITUWidget*)child;
                if (i == item_index)
                {
                    itcTreeRemove(child);
                    ituComboTableLoopExit(child);
                    vFound = true;
					//printf("[ituComboTableDelItem]item_index=%d vFound=%d\n",item_index,vFound);
                    break;
                }
                i++;
            }
			*/
			
			ITCTree *tree = (ITCTree*)cbt;
			ITCTree* child = tree->child;
			int i = 0;
			bool vFound = false;
			for (child = tree->child; child; child = child->sibling)
			{
				ITUWidget* obj = (ITUWidget*)child;
				if (obj)
				{
					if (i == item_index)
					{
						itcTreeRemove(child);
						ituComboTableLoopExit(child);
						vFound = true;
						//printf("[ituComboTableDelItem]item_index=%d vFound=%d\n",item_index,vFound);
						break;
					}
					i++;
				}
				else
					continue;
			}
			//=======================================================

            if (vFound)
            {
                int x = 0;
                for (x = i; x < cbt->itemcount; x++)
                {
                    if (((x + 1) < ITU_WIDGET_CHILD_MAX) && ((x + 1) < cbt->itemcount))
                    {
                        cbt->itemArr[x] = cbt->itemArr[x + 1];
                    }
                    if ((x + 1) >= cbt->itemcount)
                    {
                        cbt->itemArr[x] = NULL;
                    }
                }
                if (cbt->itemcount > 0)
                {
                    cbt->itemcount--;
                }

                if (item_index == cbt->selectIndex)
                {
                    cbt->selectIndex = -1;
                    cbt->lastselectIndex = -1;
                }
                //ituWidgetUpdate(cbt, ITU_EVENT_LAYOUT, 0, 0, 0);//chpark.24.03.11
                
                //ituComboTableGotoPage(cbt, cbt->currPage);
                //if (item_index < (cbt->currPage * cbt->pagemaxitem))
                //  ituComboTableGotoPage(cbt, cbt->currPage);
            }
			else
			{
				//printf("[ituComboTableDelItem][can't find] item_index=%d vFound=%d\n",item_index,vFound);
			}
        }
    }
}

int ituComboTableGetItemIndexOfWidget(ITUComboTable* cbt, ITUWidget* widget, bool currPage)
{
    if (cbt && widget)
    {
        int i = 0;
        int maxloop = (currPage) ? (cbt->pagemaxitem) : (ITU_WIDGET_CHILD_MAX);
        //use currpage to calculate start index of current page
        int p = (currPage) ? (cbt->currPage * cbt->pagemaxitem) : (0);
        const ITUWidget* cbtWidget = (ITUWidget*)cbt;
        for (i = 0; i < maxloop; i++)
        {
            if (cbt->itemArr[p])
            {
                const ITUWidget* item = cbt->itemArr[p];
                ITCTree* child = (ITCTree*)widget;
                while (child->parent)
                {
                    const ITUWidget * parent = (ITUWidget *)(child->parent);
                    if (parent == item)
                    {
                        return p;
                    }
                    else if (parent == cbtWidget)
                    {
                        break;
                    }
                    else
                    {
                        child = child->parent;
                    }
                }
            }
            p++;
        }
        return -1;
    }
    else
    {
        return -1;
    }
}

int ituComboTableGetCurrentPageNumber(ITUComboTable* cbt)
{
    if (cbt)
    {
        return cbt->currPage;
    }
    else
    {
        return -1;
    }
}

int ituComboTableGetSelectedIndex(ITUComboTable* cbt, bool current)
{
    if (cbt)
    {
        if (current)
        {
            return cbt->selectIndex;
        }
        else
        {
            return cbt->lastselectIndex;
        }
    }
    else
    {
        return -1;
    }
}
