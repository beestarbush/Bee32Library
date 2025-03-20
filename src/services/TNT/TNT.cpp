#include "TNT.h"

#include <hal/WIFI/WIFI.h>

#define CLI_CHARACTER       ("# ")
#define TCP_PORT            (8888)

namespace SSL::TNT
{
    Service::Service(HAL::WIFI::Driver& wifi) :
        ICLI::Listener(),
        _input(),
        _writeBuffer(),
        _wifi(wifi),
        _server(TCP_PORT),
        _client(),
        _itnt(nullptr),
        _state(STATE_WAIT_FOR_WIFI)
    {
    }

    void Service::begin()
    {
        _state = STATE_WAIT_FOR_WIFI;
    }

    void Service::end()
    {
        _client.stop();
        _server.stop();
    }

    void Service::run()
    {
        if (_itnt == nullptr)
        {
            // When no registered TNT is available, no need to handle the TNT.
            return;
        }

        switch (_state)
        {
            case STATE_WAIT_FOR_WIFI:
            {
                if (_wifi.getConnected() || _wifi.isAccessPoint())
                {
                    _server.begin();
                    _state = STATE_LISTEN;
                }
            }
            break;

            case STATE_LISTEN:
            {
                if (!_wifi.getConnected() && !_wifi.isAccessPoint())
                {
                    _state = STATE_CLOSING;
                    break;
                }

                _client = _server.available();
                if (!_client)
                {
                    break;
                }
                if (!_client.connected())
                {
                    break;
                }
                _state = STATE_ESTABLISHED;
            }
            break;

            case STATE_ESTABLISHED:
            {
                if ((!_wifi.getConnected() && !_wifi.isAccessPoint()) ||
                    !_client.connected())
                {
                    _state = STATE_CLOSING;
                    break;
                }

                if (_client.available())
                {
                    char data = _client.read();
                    if (data != '\n')
                    {
                        // _client.write(data);
                        _input.concat(data);
                    }

                    if (data == '\n')
                    {
                        _client.write(data);

                        if (_itnt != nullptr)
                        {
                            _itnt->OnTntDataReadyCallback(_input);
                        }
                        _input.clear();
                        _client.write(CLI_CHARACTER);
                    }
                }
            }
            break;

            case STATE_CLOSING:
            {
                _client.stop();
                _server.stop();

                _state = STATE_WAIT_FOR_WIFI;
            }
            break;

            default:
            {
                break;
            }
        }
    }

    void Service::registerITnt(TNT::Listener* TNT)
    {
        _itnt = TNT;
    }

    void Service::unregisterITnt()
    {
        _itnt = nullptr;
    }

    void Service::write(const char data[], bool newline)
    {
        if (!_client)
        {
            return;
        }

        if (newline)
        {
            _client.write(data);
            _client.write("\n\r");
        }
        else
        {
            _client.write(data);
        }
    }

    void Service::write(const String &data, bool newline)
    {
        if (!_client)
        {
            return;
        }

        data.toCharArray(_writeBuffer, TNT_WRITE_BUFFER_SIZE);

        if (newline)
        {
            _client.write(_writeBuffer, TNT_WRITE_BUFFER_SIZE);
            _client.write("\n\r");
        }
        else
        {
            _client.write(_writeBuffer, TNT_WRITE_BUFFER_SIZE);
        }
        memset(_writeBuffer, 0, TNT_WRITE_BUFFER_SIZE);
    }

    void Service::write(char data, bool newline)
    {
        if (!_client)
        {
            return;
        }

        if (newline)
        {
            _client.write(data);
            _client.write("\n\r");
        }
        else
        {
            _client.write(data);
        }
    }

    void Service::write(bool data, bool newline)
    {
        if (!_client)
        {
            return;
        }

        if (newline)
        {
            _client.write(data);
            _client.write("\n\r");
        }
        else
        {
            _client.write(data);
        }
    }

    void Service::write(uint32_t data, bool newline)
    {
        if (!_client)
        {
            return;
        }

        if (newline)
        {
            _client.write(data);
            _client.write("\n\r");
        }
        else
        {
            _client.write(data);
        }
    }

    void Service::write(uint64_t data, bool newline)
    {
        if (!_client)
        {
            return;
        }

        if (newline)
        {
            _client.write(data);
            _client.write("\n\r");
        }
        else
        {
            _client.write(data);
        }
    }

    void Service::write(void)
    {
        if (!_client)
        {
            return;
        }

        _client.write("\n\r");
    }
}