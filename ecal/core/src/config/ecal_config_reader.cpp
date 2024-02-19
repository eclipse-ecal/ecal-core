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
 * @brief  Global config class
**/

#include <cctype>
#include <ecal/ecal_defs.h>
#include <ecal/ecal_os.h>

#include "ecal_def.h"
#include "ecal_config_reader.h"
#include "ecal_global_accessors.h"
#include "util/getenvvar.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>

#include <ecal_utils/filesystem.h>
#include <string>
#include <vector>

#ifdef ECAL_OS_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#endif

#if ECAL_CORE_CONFIG_INIFILE
#include <SimpleIni.h>
#endif

namespace
{
#ifdef ECAL_OS_WINDOWS
  const char path_sep('\\');
#endif /* ECAL_OS_WINDOWS */
#ifdef ECAL_OS_LINUX
  const char path_sep('/');
#endif /* ECAL_OS_LINUX */

  bool fileexists(const std::string& fname_)
  {
    const std::ifstream infile(fname_);
    return infile.good();
  }

  bool direxists(const std::string& path_)
  {
    const EcalUtils::Filesystem::FileStatus status(path_, EcalUtils::Filesystem::Current);
    return (status.IsOk() && (status.GetType() == EcalUtils::Filesystem::Type::Dir));
  }

  void createdir(const std::string& path_)
  {
    EcalUtils::Filesystem::MkDir(path_, EcalUtils::Filesystem::Current);
  }

  std::string eCALDataEnvPath()
  {
    std::string ecal_data_path = getEnvVar("ECAL_DATA");
    if (!ecal_data_path.empty())
    {
      if (ecal_data_path.back() != path_sep)
        ecal_data_path += path_sep;
    }
    return ecal_data_path;
  }

  std::string eCALDataCMakePath()
  {
    std::string cmake_data_path;
#ifdef ECAL_OS_LINUX
    std::string ecal_install_config_dir(ECAL_INSTALL_CONFIG_DIR);
    std::string ecal_install_prefix(ECAL_INSTALL_PREFIX);

    if ((!ecal_install_config_dir.empty() && (ecal_install_config_dir[0] == path_sep))
      || ecal_install_prefix.empty())
    {
      cmake_data_path = ecal_install_config_dir;
    }
    else if (!ecal_install_prefix.empty())
    {
      cmake_data_path = ecal_install_prefix + path_sep + ecal_install_config_dir;
    }
    if (cmake_data_path.back() != path_sep) { cmake_data_path += path_sep; }
#endif /* ECAL_OS_LINUX */  
    return cmake_data_path;
  }

  std::string eCALDataSystemPath()
  {
    std::string system_data_path;
#ifdef ECAL_OS_WINDOWS
    system_data_path = getEnvVar("ProgramData");
    if (!system_data_path.empty())
    {
      if (system_data_path.back() != path_sep)
      {
        system_data_path += path_sep;
      }
      system_data_path += std::string("eCAL") + path_sep;
    }
#endif /* ECAL_OS_WINDOWS */

#ifdef ECAL_OS_LINUX
    system_data_path = "/etc/ecal/";
#endif /* ECAL_OS_LINUX */
    return system_data_path;
  }


  // 1. The path is not empty
  // 2. The ecal.ini exists in that directory
  bool IsValidConfigFilePath(const std::string& path)
  {
    if (path.empty()) { return false; }

    // check existence of ecal.ini file
    const EcalUtils::Filesystem::FileStatus ecal_ini_status(path + std::string(ECAL_DEFAULT_CFG), EcalUtils::Filesystem::Current);
    if (ecal_ini_status.IsOk() && (ecal_ini_status.GetType() == EcalUtils::Filesystem::Type::RegularFile))
    {
      return true;
    }

    return false;
  }

}

namespace eCAL
{
  namespace Util
  {
    ECAL_API std::string GeteCALHomePath()
    {
      std::string home_path;

#ifdef ECAL_OS_WINDOWS
      // check ECAL_HOME
      home_path = getEnvVar("ECAL_HOME");
      if (!home_path.empty())
      {
        if (*home_path.rbegin() != path_sep) home_path += path_sep;
      }
      if (!std::string(ECAL_HOME_PATH_WINDOWS).empty()) //-V815
      {
        home_path += path_sep;
        home_path += ECAL_HOME_PATH_WINDOWS;
      }
#endif /* ECAL_OS_WINDOWS */

#ifdef ECAL_OS_LINUX
      const char *hdir;
      if ((hdir = getenv("HOME")) == NULL) {
        hdir = getpwuid(getuid())->pw_dir;
      }
      home_path += hdir;
      if (!std::string(ECAL_HOME_PATH_LINUX).empty())
      {
        home_path += "/";
        home_path += ECAL_HOME_PATH_LINUX;
      }
#endif /* ECAL_OS_LINUX */

      // create if not exists
      if (!direxists(home_path))
      {
        createdir(home_path);
      }

      home_path += path_sep;
      return(home_path);
    }

    ECAL_API std::string GeteCALConfigPath()
    {
      // -----------------------------------------------------------
      // precedence 1: ECAL_DATA variable (windows and linux)
      // -----------------------------------------------------------
      const std::string ecal_data_path{ eCALDataEnvPath() };
      
      // -----------------------------------------------------------
      // precedence 2:  cmake configured data paths (linux only)
      // -----------------------------------------------------------
      const std::string cmake_data_path{ eCALDataCMakePath() };

      // -----------------------------------------------------------
      // precedence 3: system data path 
      // -----------------------------------------------------------
      const std::string system_data_path(eCALDataSystemPath());

      // Check for first directory which contains the ini file.
      std::vector<std::string> search_directories{ ecal_data_path, cmake_data_path, system_data_path };

      auto it = std::find_if(search_directories.begin(), search_directories.end(), IsValidConfigFilePath);
      // We should have encountered a valid path
      if (it != search_directories.end())
        return (*it);

      // If valid path is not encountered, defaults should be used
      return std::string("");
    }

    ECAL_API std::string GeteCALUserSettingsPath()
    {
      std::string settings_path;
#ifdef ECAL_OS_WINDOWS
      settings_path = GeteCALConfigPath();
#endif /* ECAL_OS_WINDOWS */

#ifdef ECAL_OS_LINUX
      settings_path = GeteCALHomePath();
#endif /* ECAL_OS_LINUX */
      settings_path += std::string(ECAL_SETTINGS_PATH);

      if (!direxists(settings_path))
      {
        createdir(settings_path);
      }

      settings_path += path_sep;
      return(settings_path);
    }

    ECAL_API std::string GeteCALLogPath()
    {
      std::string log_path;
#ifdef ECAL_OS_WINDOWS
      log_path = GeteCALConfigPath();
#endif /* ECAL_OS_WINDOWS */

#ifdef ECAL_OS_LINUX
      log_path = GeteCALHomePath();
#endif /* ECAL_OS_LINUX */

      log_path += std::string(ECAL_LOG_PATH);

      if (!direxists(log_path))
      {
        createdir(log_path);
      }

      log_path += path_sep;
      return(log_path);
    }

    ECAL_API std::string GeteCALActiveIniFile()
    {
      std::string ini_file = GeteCALConfigPath();
      ini_file += ECAL_DEFAULT_CFG;
      return ini_file;
    }

    ECAL_API std::string GeteCALDefaultIniFile()
    {
      return GeteCALActiveIniFile();
    }
  }

  ////////////////////////////////////////////////////////
  // CConfigImpl
  ////////////////////////////////////////////////////////
#if ECAL_CORE_CONFIG_INIFILE

  class CConfigImpl : public CSimpleIni
  {
  public:
    CConfigImpl() = default;
    virtual ~CConfigImpl() = default;

    void OverwriteKeys(const std::vector<std::string>& key_vec_)
    {
      m_overwrite_keys = key_vec_;
    }

    void AddFile(std::string& file_name_)
    {
      std::string cfg_fname = file_name_;
      if (!fileexists(cfg_fname))
      {
        cfg_fname = Util::GeteCALConfigPath() + cfg_fname;
      }

      // load settings config file
      bool loaded(false);
      if (fileexists(cfg_fname))
      {
        // apply file name to manager
        loaded = LoadFile(cfg_fname.c_str()) == 0;        
      }

      // load error ?
      if(!loaded)
      { 
        std::cout << "eCAL::Config - Could not load ini file, using defaults. File name : " << cfg_fname << std::endl;
      }
      else
      {
        // return full name
        file_name_ = cfg_fname;
      }

      // update command line keys
      for (const auto& full_key : m_overwrite_keys)
      {
         auto sec_pos = full_key.find_last_of('/');
         if (sec_pos == std::string::npos) continue;
         const std::string section = full_key.substr(0, sec_pos);
         std::string key     = full_key.substr(sec_pos+1);

         auto val_pos = key.find_first_of(':');
         if (val_pos == std::string::npos) continue;
         const std::string value = key.substr(val_pos+1);
         key = key.substr(0, val_pos);

         const SI_Error err = SetValue(section.c_str(), key.c_str(), value.c_str());
         if (err == SI_FAIL)
         {
            std::cout << "Error: Could not overwrite key " << key << " in section " << section << ".";
         }
      }
    }
  protected:
    std::vector<std::string> m_overwrite_keys;
  };

#else // ECAL_CORE_CONFIG_INIFILE

  class CConfigImpl
  {
  public:
    CConfigImpl() = default;
    virtual ~CConfigImpl() = default;

    void OverwriteKeys(const std::vector<std::string>& /*key_vec_*/) {}
    void AddFile(std::string& /*file_name_*/) {}

    std::string GetValue(const std::string& /*section_*/, const std::string& /*key_*/, const std::string& default_) { return default_;}
    long GetLongValue(const std::string& /*section_*/, const std::string& /*key_*/, long default_)                  { return default_; }
    double GetDoubleValue(const std::string& /*section_*/, const std::string& /*key_*/, double default_)            { return default_; }
  };

#endif // ECAL_CORE_CONFIG_INIFILE

  ////////////////////////////////////////////////////////
  // CConfigBase
  ////////////////////////////////////////////////////////
  CConfig::CConfig() :
    m_impl(nullptr)
  {
    m_impl = std::make_unique<CConfigImpl>();
  }

  CConfig::~CConfig() = default;

  void CConfig::OverwriteKeys(const std::vector<std::string>& key_vec_)
  {
    m_impl->OverwriteKeys(key_vec_);
  }

  void CConfig::AddFile(std::string& ini_file_)
  {
    m_impl->AddFile(ini_file_);
  }

  bool CConfig::Validate()
  {
    // ------------------------------------------------------------------
    // UDP and TCP publlisher mode should not set to "auto (2)" both
    // 
    // [publisher]
    // use_tcp    = 2
    // use_udp_mc = 2
    // ------------------------------------------------------------------
    {
      const int use_tcp    = get("publisher", "use_tcp",    0);
      const int use_udp_mc = get("publisher", "use_udp_mc", 0);
      if ((use_tcp == 2) && (use_udp_mc == 2))
      {
        std::cerr << "eCAL config error: to set [publisher/use_tcp] and [publisher/use_udp_mc] both on auto mode (2) is not allowed" << std::endl;
        return false;
      }
    }

    return true;
  }

  bool CConfig::get(const std::string& section_, const std::string& key_, bool default_)
  {
    std::string default_s("false");
    if (default_) default_s = "true";
    std::string value_s = m_impl->GetValue(section_.c_str(), key_.c_str(), default_s.c_str());
    // cause warning C4244 with VS2017
    //std::transform(value_s.begin(), value_s.end(), value_s.begin(), tolower);
    std::transform(value_s.begin(), value_s.end(), value_s.begin(),
      [](char c) {return static_cast<char>(::tolower(c)); });

    if ((value_s == "true") || (value_s == "1")) return true;
    return false;
  }

  int CConfig::get(const std::string& section_, const std::string& key_, int default_)
  {
    return static_cast<int>(m_impl->GetLongValue(section_.c_str(), key_.c_str(), static_cast<long>(default_)));
  }

  double CConfig::get(const std::string& section_, const std::string& key_, double default_)
  {
    return m_impl->GetDoubleValue(section_.c_str(), key_.c_str(), default_);
  }

  std::string CConfig::get(const std::string& section_, const std::string& key_, const char* default_)
  {
    return m_impl->GetValue(section_.c_str(), key_.c_str(), default_);
  }

  bool CfgGetBool(const std::string& section_, const std::string& key_, bool default_ /*= false*/)
  {
    return g_config()->get(section_, key_, default_);
  }

  int CfgGetInt(const std::string& section_, const std::string& key_, int default_ /*= 0*/)
  {
    return g_config()->get(section_, key_, default_);
  }

  double CfgGetDouble(const std::string& section_, const std::string& key_, double default_ /*= 0.0*/)
  {
    return g_config()->get(section_, key_, default_);
  }

  std::string CfgGetString(const std::string& section_, const std::string& key_, const char* default_ /*= ""*/)
  {
    return g_config()->get(section_, key_, default_);
  }
}
