#include "error.h"

Error::Error()
{
    code = e_NO_ERROR;
}

/*Error& Error::operator =(Error &paError)
{
    if (this == &paError)
        return *this;

    code = paError.getCode();
    text = paError.getText();

    return *this;
}*/

Error& Error::operator =(Error paError)
{
    code = paError.getCode();
    text = paError.getText();

    return *this;
}

bool Error::operator ==(d_ERRORCODE paErrorCode)
{
    return code == paErrorCode;
}

bool Error::operator !=(d_ERRORCODE paErrorCode)
{
    return code != paErrorCode;
}

bool operator ==(d_ERRORCODE paErrorCode, Error &paError)
{
    return paError == paErrorCode;
}

bool operator !=(d_ERRORCODE paErrorCode, Error &paError)
{
    return paError != paErrorCode;
}
