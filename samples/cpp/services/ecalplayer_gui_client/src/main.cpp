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

#include "ecalplayer_gui_client.h"
#include <QtWidgets/QApplication>
#include <ecal/ecal.h>

int main(int argc, char *argv[])
{

  // Just make sure that eCAL is initialized
  eCAL::Initialize(0, nullptr, "EcalPlayGuiClient", eCAL::Init::Default);

  QApplication a(argc, argv);

  a.setOrganizationName      ("Continental");
  a.setOrganizationDomain    ("continental-corporation.com");
  a.setApplicationName       ("ecalplay_gui_client");
  a.setApplicationDisplayName("eCAL Player GUI client");

  EcalplayGuiClient* w = new EcalplayGuiClient();
  w->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);

  w->show();
  int return_code = a.exec();

  eCAL::Finalize();

  return return_code;
}
