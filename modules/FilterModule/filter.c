#include "filter.h"

IOTHUB_MESSAGE_HANDLE FilterMessage(IOTHUB_MESSAGE_HANDLE message, int threshold)
{
    unsigned const char *messageBody;
    size_t contentSize;

    if (IoTHubMessage_GetByteArray(message, &messageBody, &contentSize) != IOTHUB_MESSAGE_OK)
    {
        return NULL;
    }

    IOTHUB_MESSAGE_HANDLE filtered_message = IoTHubMessage_CreateFromString(messageBody);

    JSON_Value *rootValue = json_parse_string(messageBody);
    JSON_Object *rootObject = json_value_get_object(rootValue);
    JSON_Object *machineObject = json_object_get_object(rootObject, "machine");
    double temperature = json_object_dotget_number(machineObject, "temperature");

    if (temperature > threshold)
    {
        MAP_HANDLE mapProperties = IoTHubMessage_Properties(message);

        if (mapProperties != NULL)
        {
            const char *const *keys;
            const char *const *values;
            size_t propertyCount = 0;
            if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount) == MAP_OK)
            {
                if (propertyCount > 0)
                {
                    size_t index;
                    for (index = 0; index < propertyCount; index++)
                    {
                        IoTHubMessage_SetProperty(filtered_message, keys[index], values[index]);
                    }
                }
            }

            const char *originContentType = IoTHubMessage_GetContentTypeSystemProperty(message);
            const char *originContentEncoding = IoTHubMessage_GetContentEncodingSystemProperty(message);

            if (originContentType != NULL)
            {
                IoTHubMessage_SetContentTypeSystemProperty(filtered_message, originContentType);
            }
            else
            {
                IoTHubMessage_SetContentTypeSystemProperty(filtered_message, "application/json");
            }

            if (originContentEncoding != NULL)
            {
                IoTHubMessage_SetContentEncodingSystemProperty(filtered_message, originContentEncoding);
            }
            else
            {
                IoTHubMessage_SetContentEncodingSystemProperty(filtered_message, "utf-8");
            }
        }

        IoTHubMessage_SetProperty(filtered_message, "MessageType", "Alert");

        return filtered_message;
    }
    else
    {
        return NULL;
    }
}
