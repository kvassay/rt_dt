#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <sstream>

#include "Core/errorcodes.h"
#include "Core/definitions.h"

class Error
{
private:
    d_ERRORCODE code;
    std::string text;
public:
    Error();
    void set(d_ERRORCODE paCode, std::string *paText = NULL)
    {
        code = paCode;
        if (paText != NULL)
            text = *paText;
        else
            text.clear();
    }
    void set(d_ERRORCODE paCode, std::stringstream *paText)
    {
        code = paCode;
        if (paText != NULL)
            text = paText->str();
        else
            text.clear();
    }
    void reset()
    {
        code = e_NO_ERROR;
        text.clear();
    }

    d_ERRORCODE getCode() { return code; }
    std::string getText() { return text; }

    void prependText(std::string *paText = NULL)
    {
        if (paText != NULL)
            text = *paText + text;
    }
    void prependText(const char* paText)
    {
        if (paText != NULL)
        {
            std::string pomStr(paText);
            text = pomStr + text;
        }
    }
    void prependText(std::stringstream *paText)
    {
        if (paText != NULL)
            text = paText->str() + text;
    }
    //Error& operator =(Error& paError);
    Error& operator =(Error paError);

    operator bool() { return code != e_NO_ERROR; }


    bool operator ==(d_ERRORCODE paErrorCode);
    bool operator !=(d_ERRORCODE paErrorCode);
    friend bool operator ==(d_ERRORCODE paErrorCode, Error &paError);
    friend bool operator !=(d_ERRORCODE paErrorCode, Error &paError);
};

#endif // ERROR_H
