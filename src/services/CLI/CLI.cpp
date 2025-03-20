#include "CLI.h"

#define CLI_CHARACTER     "# "

namespace SSL::CLI
{
    Service::Service() :
        TTY::Listener(),
        TNT::Listener(),
        HTTP::Listener(),
        _cli(),
        _icli(nullptr),
        _cmdHelp()
    {
    }

    void Service::begin()
    {
        _cmdHelp = _cli.addCommand("help");

        onAddCommands();
    }

    void Service::end()
    {
    }

    void Service::run()
    {
        if (_icli == nullptr)
        {
            // When no registered CLI is available, no need to handle the CLI.
            return;
        }

        if (_cli.available())
        {
            Command c = _cli.getCmd();

            int argNum = c.countArgs();

            for (int i = 0; i<argNum; ++i)
            {
                Argument arg = c.getArgument(i);
                // if(arg.isSet()) {
                _icli->write(arg.toString(), false);
                _icli->write(' ', false);
                // }
            }

            _icli->write();

            if (c == _cmdHelp)
            {
                _icli->write("Usage:");
                _icli->write(_cli.toString());
            }
            else if (!onCommandCallback(c))
            {
                // Returned false, will give the help now.
                _icli->write("Incorrect command, usage:");
                _icli->write(_cli.toString());
            }
            _icli->write(CLI_CHARACTER, false);
        }

        if (_cli.errored())
        {
            CommandError cmdError = _cli.getError();

            _icli->write("ERROR: ", false);
            _icli->write(cmdError.toString());

            if (cmdError.hasCommand())
            {
                _icli->write("Usage: \"", false);
                _icli->write(cmdError.getCommand().toString());
            }
            _icli->write(CLI_CHARACTER, false);
        }
    }

    void Service::registerICli(ICLI::Listener* cli)
    {
        _icli = cli;
    }

    void Service::unregisterICli()
    {
        _icli = nullptr;
    }

    void Service::OnTtyDataReadyCallback(String data)
    {
        _cli.parse(data);
    }

    void Service::OnTntDataReadyCallback(String data)
    {
        _cli.parse(data);
    }

    void Service::OnHttpDataReadyCallback(String data)
    {
        _cli.parse(data);
    }
}