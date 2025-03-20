#ifndef _SERVICES_CLI_H
#define _SERVICES_CLI_H

#include "services/TTY/TTY.h"
#include "services/TNT/TNT.h"
#include "services/HTTP/HTTP.h"

#include <WString.h>
#include <SimpleCLI.h>

namespace SSL::CLI
{
    enum Interface
    {
        TELETYPE = 0,
        TELNET,
        HYPERTEXT,
        COUNT
    };
    class Service : public TTY::Listener, public TNT::Listener, public HTTP::Listener
    {
        public:
            Service();
            ~Service() = default;

            void begin();
            void end();

            void run();

            void registerICli(ICLI::Listener* cli);
            void unregisterICli();

        protected:
            SimpleCLI _cli;
            ICLI::Listener* _icli;
            Command _cmdHelp;

            virtual void onAddCommands() = 0;
            virtual bool onCommandCallback(Command& command) = 0;

        private:
            void OnTtyDataReadyCallback(String data);
            void OnTntDataReadyCallback(String data);
            void OnHttpDataReadyCallback(String data);
    };

    //         void OnTtyDataReadyCallback(String data);
    //         void OnTntDataReadyCallback(String data);
    //         void OnHttpDataReadyCallback(String data);

    //     private:
    //         SimpleCLI _cli;
    //         ICLI::Listener* _icli;
    //         Command _cmdHelp;

    //         // WIFI
    //         Command _cmdWifi;
    //         WIFI::Driver& _wifi;
    //         void OnWiFiCommandCallback(Command* cmd);

    //         // VER
    //         Command _cmdVer;
    //         VER::Service& _ver;
    //         void OnVerCommandCallback(Command* cmd);

    //         // CFG
    //         Command _cmdCfg;
    //         NVS::Service& _cfg;
    //         void OnCfgCommandCallback(Command* cmd);

    //         // DAT
    //         Command _cmdDat;
    //         NVS::Service& _dat;
    //         void OnDatCommandCallback(Command* cmd);
    //         void OnNvsHandling(NVS::Service* nvs, Argument keyArg, Argument valueArg, Argument typeArg, bool write, bool read, bool erase, bool contains);
    // };
}

#endif
