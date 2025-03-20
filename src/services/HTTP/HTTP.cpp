#include "HTTP.h"

#include <hal/WIFI/WIFI.h>

#define CLI_CHARACTER       ("# ")
#define TCP_PORT            (80)

#define HTTP_OK                 ("HTTP/1.1 200 OK")
#define HTTP_PARSE_ERROR        ("HTTP/1.1 400 Bad Request")
#define HTTP_CONTENT_TYPE       ("Content-type:text/html")
#define HTTP_CONNECTION_CLOSE   ("Connection:close")

namespace SSL::HTTP
{
    Service::Service(HAL::WIFI::Driver& wifi) :
        _input(),
        _writeBuffer(),
        _wifi(wifi),
        _server(TCP_PORT),
        _client(),
        _ihttp(nullptr),
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
        if (_ihttp == nullptr)
        {
            // When no registered HTTP is available, no need to handle the HTTP.
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

                    // We only wait for the newline of the header, when the header is received
                    // we will parse it and return a response code and close the connection.
                    if (data == '\n')
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        bool parseResult = parseGetRequest();
                        if (parseResult)
                        {
                            _client.println(HTTP_OK);
                        }
                        else
                        {
                            _client.println(HTTP_PARSE_ERROR);
                            _client.println("Unable to parse command and/or values.");
                        }
                        _client.println(HTTP_CONTENT_TYPE);
                        _client.println(HTTP_CONNECTION_CLOSE);
                        _client.println();

                        if (_ihttp != nullptr && parseResult)
                        {
                            _ihttp->OnHttpDataReadyCallback(_input);
                        }

                        _input.clear();
                        _client.write(CLI_CHARACTER);
                        _state = STATE_CLOSING;
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

    void Service::registerIHttp(HTTP::Listener* http)
    {
        _ihttp = http;
    }

    void Service::unregisterIHttp()
    {
        _ihttp = nullptr;
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
            _client.write("</br>");
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

        String httpData(data);
        httpData.replace("\n", "</br>");
        httpData.toCharArray(_writeBuffer, HTTP_WRITE_BUFFER_SIZE);

        if (newline)
        {
            _client.write(_writeBuffer, HTTP_WRITE_BUFFER_SIZE);
            _client.write("</br>");
        }
        else
        {
            _client.write(_writeBuffer, HTTP_WRITE_BUFFER_SIZE);
        }
        memset(_writeBuffer, 0, HTTP_WRITE_BUFFER_SIZE);
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
            _client.write("</br>");
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

        String output(data);
        write(output);
    }

    void Service::write(uint32_t data, bool newline)
    {
        if (!_client)
        {
            return;
        }

        String output(data);
        write(output);
    }

    void Service::write(uint64_t data, bool newline)
    {
        if (!_client)
        {
            return;
        }

        String output(data);
        write(output);
    }

    void Service::write(void)
    {
        if (!_client)
        {
            return;
        }

        _client.write("</br>");
    }

    bool Service::parseGetRequest()
    {
        // Remove any trailing data from the HTTP header.
        int32_t trailingDataIndex = _input.indexOf("HTTP/1.1");
        if (trailingDataIndex != -1)
        {
            // If we found the railing data and modify the input data,
            // however when it is not found we continue with data as is.
            _input = _input.substring(0, trailingDataIndex);
        }

        int nameIndex = _input.indexOf('/');
        int valueIndex = _input.indexOf('/', nameIndex + 1);

        String data;
        // Handle the case where name and at least one value is available.
        if (nameIndex != -1 && valueIndex != -1)
        {
            data.concat(_input.substring(nameIndex + 1, valueIndex));
            data.concat(' ');

            // Lookup values based on the slash.
            int start = valueIndex + 1;
            int end = _input.indexOf('/', start);
            while (end != -1)
            {
                data.concat(_input.substring(start, end));
                data.concat(' ');
                start = end + 1;
                end = _input.indexOf('/', start);
            }

            // Add the last value if it exists.
            if (start < _input.length())
            {
                data.concat(_input.substring(start));
            }
        } 
        // Handle the case where only the field name is present and no additional values.
        else if (nameIndex != -1)
        {
            if ((_input.length() - nameIndex) < 3)
            {
                // Verify that the name is at least 3 characters, when not we show the
                // help.
                data = "help";
            }
            else
            {
                data.concat(_input.substring(nameIndex + 1));
            }
        }
        else
        {
            return false;
        }

        _input = data;

        return true;
    }
}