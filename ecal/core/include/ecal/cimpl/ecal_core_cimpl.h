/* ========================= eCAL LICENSE =================================
 *
 * Copyright (C) 2016 - 2019 Continental Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ========================= eCAL LICENSE =================================
*/

/**
 * @file   ecal_core_cimpl.h
 * @brief  eCAL core function c interface
**/

#ifndef ecal_core_cimpl_h_included
#define ecal_core_cimpl_h_included

#include <ecal/ecal_os.h>
#include <ecal/ecalc_types.h>

#include "ecal_init_cimpl.h"

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/
  /**
   * @brief  Get eCAL version string. 
   *
   * @return  Full eCAL version string. 
  **/
  ECALC_API const char* eCAL_GetVersionString();

  /**
   * @brief  Get eCAL version date. 
   *
   * @return  Full eCAL version date string. 
  **/
  ECALC_API const char* eCAL_GetVersionDateString();

  /**
   * @brief  Get eCAL version as separated integer values. 
   *
   * @param [out] major_  The eCAL major version number.
   * @param [out] minor_  The eCAL minor version number.
   * @param [out] patch_  The eCAL patch version number.
   *
   * @return  Zero if succeeded.
  **/
  ECALC_API int eCAL_GetVersion(int* major_, int* minor_, int* patch_);

  /**
   * @brief Initialize eCAL API.
   *
   * @param argc_        Number of command line arguments. 
   * @param argv_        Array of command line arguments. 
   * @param unit_name_   Defines the name of the eCAL unit. 
   * @param components_  Defines which component to initialize.
   *
   * @return Zero if succeeded, 1 if already initialized, -1 if failed.
  **/
  ECALC_API int eCAL_Initialize(int argc_, char **argv_, const char *unit_name_, unsigned int components_);

  /**
   * @brief  Set/change the unit name of current module.
   *
   * @param unit_name_  Defines the name of the eCAL unit. 
   *
   * @return  Zero if succeeded.
  **/
  ECALC_API int eCAL_SetUnitName(const char *unit_name_);

  /**
   * @brief Finalize eCAL API.
   *
   * @param components_  Defines which component to initialize (not yet supported).
   *
   * @return Zero if succeeded, 1 if already finalized, -1 if failed.
  **/
  ECALC_API int eCAL_Finalize(unsigned int components_);

  /**
   * @brief Check eCAL initialize state.
   *
   * @param component_  Check specific component or 0 for general state of eCAL core.
   *
   * @return None zero if eCAL is initialized.
  **/
  ECALC_API int eCAL_IsInitialized(unsigned int component_);

  /**
   * @brief Return the eCAL process state.
   *
   * @return  None zero if eCAL is in proper state. 
  **/
  ECALC_API int eCAL_Ok();

  /**
   * @brief  Free an eCAL memory block allocated by functions like
   *         eCAL_Monitoring_GetMonitoring, eCAL_Monitoring_GetLogging, 
   *         eCAL_Sub_Receive ... that use 'ECAL_ALLOCATE_4ME' as 
   *         buffer length parameter and let eCAL allocate 
   *         the memory internally.
   *
   **/
  /**
   * @code
   *            // let eCAL allocate memory for the subscriber buffer and return the pointer to 'buf'
   *            long long time     = 0;
   *            int       time_out = 100;   // ms
   *            void*     buf      = NULL;
   *            int       buf_len  = eCAL_Sub_Receive_Alloc(subscriber_handle, &buf, &time, timeout);
   *            if(buf_len > 0)
   *            {
   *              ...
   *              // PROCESS THE BUFFER CONTENT HERE
   *              ...
   *              // finally free the allocated memory
   *              eCAL_FreeMem(buf);
   *            }
   * @endcode
  **/
  ECALC_API void eCAL_FreeMem(void* mem_);
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*ecal_core_cimpl_h_included*/
