/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*============================ INCLUDES ======================================*/
#define __SIMPLE_LIST_INHERIT__
#define __TEXT_TRACKING_LIST_IMPLEMENT__
#define __ARM_2D_HELPER_LIST_VIEW_INHERIT__

#include "./text_tracking_list.h"

#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wformat-nonliteral"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif


#undef this
#define this        (*ptThis)

#undef base
#define base        (*ptBase)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static 
IMPL_PFB_ON_DRAW(__arm_2d_text_tracking_list_draw_background);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#   define __va_list    va_list
#endif


ARM_NONNULL(1,2,3)
arm_2d_err_t __text_tracking_list_init(  
                        text_tracking_list_t *ptThis, 
                        text_tracking_list_cfg_t *ptCFG,
                        arm_2d_i_list_region_calculator_t *ptCalculator)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(NULL != ptCalculator);

    //__simple_list_t *ptBase = &this.use_as__text_list_t.use_as____simple_list_t;

    this.tSettings = ptCFG->tSettings;

    arm_2d_err_t tResult = __text_list_init(&this.use_as__text_list_t, 
                                            &ptCFG->use_as__text_list_cfg_t,
                                            ptCalculator);

    if (NULL == ptCFG->use_as__text_list_cfg_t
                            .use_as____simple_list_cfg_t
                                .fnOnDrawListBackground) {
        __simple_list_set_draw_list_background_handler(
                            &this.use_as__text_list_t.use_as____simple_list_t,
                            &__arm_2d_text_tracking_list_draw_background);
    }


    if (0 == (  ARM_2D_ALIGN_LEFT 
             &  ptCFG->use_as__text_list_cfg_t
                    .use_as____simple_list_cfg_t
                        .tTextAlignment)) {
        this.tSettings.bIndicatorAutoSize = false;
    }

    return tResult;
}

static 
IMPL_PFB_ON_DRAW(__arm_2d_text_tracking_list_draw_background)
{
    ARM_2D_UNUSED(bIsNewFrame);
    
    text_tracking_list_t *ptThis = (text_tracking_list_t *)pTarget;
    text_list_t *ptBase = &this.use_as__text_list_t;

    //uint16_t hwListCount = text_tracking_list_get_list_item_count(ptThis);
    uint16_t hwSelectedID = text_tracking_list_get_selected_item_id(ptThis);

    arm_2d_region_t tSelectionRegion;

    arm_2d_size_t tStringSize;
    ARM_2D_UNUSED(tStringSize);

    if (this.tSettings.bIndicatorAutoSize) {
        tStringSize = 
            __arm_lcd_get_string_line_box(
                        text_tracking_list_get_item_string(ptThis, hwSelectedID), 
                        NULL);
    }

    __arm_2d_list_core_get_selection_region(
            &base.use_as____simple_list_t.use_as____arm_2d_list_core_t, 
            &tSelectionRegion);

    if (bIsNewFrame) {
        if (this.tSettings.bIndicatorAutoSize) {
            if (0 == this.tLastStringSize.iWidth) {
                this.tLastStringSize = tStringSize;
                this.iStringWidth = tStringSize.iWidth;
            } else {
                int32_t nResult;
                if (arm_2d_helper_time_liner_slider(this.tLastStringSize.iWidth, 
                                                    tStringSize.iWidth, 
                                                    200, 
                                                    &nResult, 
                                                    &this.lTimestamp)) {
                    this.lTimestamp = 0;
                    this.tLastStringSize = tStringSize;
                }
                this.iStringWidth = nResult;
            }
        }
    }

    if (this.tSettings.bIndicatorAutoSize) {
        tSelectionRegion.tSize.iWidth = this.iStringWidth + 4;
    } else {
        tSelectionRegion.tSize.iWidth -= 8;
    }

    if (!base.use_as____simple_list_t.tSimpleListCFG.bIgnoreBackground) {

        /* call the base method  */
        __arm_2d_simple_list_draw_background(&this.use_as__text_list_t,
                                             ptTile, 
                                             bIsNewFrame);
    }


    arm_2d_canvas(ptTile, __list_cover) {

        arm_2d_fill_colour( ptTile, 
                            &tSelectionRegion, 
                            (COLOUR_INT)this.tSettings.tIndicator.wColour);
    }

    ARM_2D_OP_WAIT_ASYNC();
    
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
void text_tracking_list_depose(text_tracking_list_t *ptThis)
{
    assert(NULL != ptThis);

    text_list_depose(&this.use_as__text_list_t);
}


ARM_NONNULL(1)
void text_tracking_list_on_frame_start(text_tracking_list_t *ptThis)
{
    assert(NULL != ptThis);
    //__simple_list_t *ptBase = &this.use_as__text_list_t.use_as____simple_list_t;

    text_list_on_frame_start(&this.use_as__text_list_t);
}

ARM_NONNULL(1,2)
arm_fsm_rt_t text_tracking_list_show(  text_tracking_list_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion, 
                                bool bIsNewFrame)
{
    assert(NULL != ptThis);
    assert(NULL != ptTile);

    return text_list_show(&this.use_as__text_list_t,
                            ptTile,
                            ptRegion,
                            bIsNewFrame);
}


ARM_NONNULL(1)
void text_tracking_list_move_selection( text_tracking_list_t *ptThis,
                                int16_t iSteps,
                                int32_t nFinishInMs)
{
    text_list_move_selection( &this.use_as__text_list_t, 
                                iSteps, 
                                nFinishInMs);
}

ARM_NONNULL(1)
uint16_t text_tracking_list_get_selected_item_id(text_tracking_list_t *ptThis)
{
    return text_list_get_selected_item_id(&this.use_as__text_list_t);
}


ARM_NONNULL(1)
__disp_string_t text_tracking_list_get_item_string(text_tracking_list_t *ptThis, uint_fast16_t hwItemID)
{
    return text_list_get_item_string(&this.use_as__text_list_t, hwItemID);
}

ARM_NONNULL(1)
uint16_t text_tracking_list_get_list_item_count(text_tracking_list_t *ptThis)
{
    return text_list_get_list_item_count(&this.use_as__text_list_t);
}

ARM_NONNULL(1)
void text_tracking_list_request_redraw_list(text_tracking_list_t *ptThis)
{
    assert(NULL != ptThis);
    text_list_request_redraw_list(&this.use_as__text_list_t);
}

ARM_NONNULL(1)
void text_tracking_list_request_redraw_selected_item(text_tracking_list_t *ptThis)
{
    assert(NULL != ptThis);
    text_list_request_redraw_selected_item(&this.use_as__text_list_t);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
