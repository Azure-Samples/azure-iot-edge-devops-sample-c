#include <tap.h>
#include "filter.h"

const char template[] = "{\"machine\":{\"temperature\":%s,\"pressure\":0}, \"ambient\":{\"temperature\":0,\"humidity\":0}}";

IOTHUB_MESSAGE_HANDLE CreateMessage(const char *temperature)
{
    char messageStr[100];
    sprintf(messageStr, template, temperature);
    return IoTHubMessage_CreateFromByteArray((const unsigned char *)messageStr, strlen(messageStr));
}

int ShouldNotFilterValuesTemperatureLessThanThreshold()
{
    IOTHUB_MESSAGE_HANDLE source = CreateMessage("24");
    IOTHUB_MESSAGE_HANDLE result = FilterMessage(source, 25);
    return result == NULL ? true : false;
}

int ShouldFilterValuesTemperatureLagerThanTreshold()
{
    IOTHUB_MESSAGE_HANDLE source = CreateMessage("26");
    IOTHUB_MESSAGE_HANDLE result = FilterMessage(source, 25);
    if (strcmp(IoTHubMessage_GetProperty(result, "MessageType"), "Alert") == 0)
    {
        return true;
    }
    

    return false;
}

int FilterLargerThanThresholdAndCopyAdditionalProperty()
{
    const char *expected = "customTestValue";
    IOTHUB_MESSAGE_HANDLE source = CreateMessage("26");
    IoTHubMessage_SetProperty(source, "customTestKey", expected);
    IOTHUB_MESSAGE_HANDLE result = FilterMessage(source, 25);

    if (strcmp(IoTHubMessage_GetProperty(result, "customTestKey"), expected) == 0)
    {
        return true;
    }
    return false;
}

int main()
{
    printf("Unit Test Start Running\n");
    plan(3);
    ok(ShouldNotFilterValuesTemperatureLessThanThreshold(), "should not filter values temperature less than threshold");
    ok(ShouldFilterValuesTemperatureLagerThanTreshold(), "should filter values temperature larger than threshold");
    ok(FilterLargerThanThresholdAndCopyAdditionalProperty(), "filter larger than threshold and copy additional property");
    done_testing();
}