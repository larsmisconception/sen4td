import re

Scale_measurement_characteristic_id = '6E400003-B5A3-F393-E0A9-E50E24DCCA89'

ScaleMeasurement = 0


def _parse_pt_measurement(data):
    stTemp = data.decode("utf-8")
    print (stTemp)
    ScaleMeasurement = float(stTemp)
    return ScaleMeasurement


class ScaleService:
    def __init__(self, client):
        self._client = client
        self._pt_measurement_callback = None

    async def enable_pt_measurement_notifications(self):
        await self._client.start_notify(Scale_measurement_characteristic_id,
                                        self._pt_measurement_notification_handler)

    async def disable_pt_measurement_notifications(self):
        await self._client.stop_notify(Scale_measurement_characteristic_id)

    def set_pt_measurement_handler(self, callback):
        self._pt_measurement_callback = callback

    def _pt_measurement_notification_handler(self, sender, data):  # pylint: disable=unused-argument
        if self._pt_measurement_callback is not None:
            self._pt_measurement_callback(_parse_pt_measurement(data))
                
    async def get_pt(self):

        return PressTempMeasurement
