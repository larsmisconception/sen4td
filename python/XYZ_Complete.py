from collections import namedtuple
import re

PressTemp_measurement_characteristic_id = '6E400003-B5A3-F393-E0A9-E50E24DCCA90'

PressTempMeasurement = namedtuple('PressTempMeasurement', ['Pressure', 'Temp', 'dfX', 'dfY', 'dfZ', 'dfAX', 'dfAY', 'dfAZ'])


def _parse_pt_measurement(data):
    stTemp = data.decode("utf-8")
    Pressure = 0;
    Temp=0;
    dfX=0;
    dfY=0;
    dfZ=0;
    st = re.split(";|:", stTemp)
    Temp=float(st[1])
    dfX=float(st[3])
    dfY=float(st[5])
    dfZ=float(st[7])
    dfAX=float(st[9])
    dfAY=float(st[11])
    dfAZ=float(st[13])
    return PressTempMeasurement(Pressure = Pressure,
                                Temp=Temp,
                                dfX=dfX,
                                dfY=dfY,
                                dfZ=dfZ,
                                dfAX=dfAX,
                                dfAY=dfAY,
                                dfAZ=dfAZ)


class XYZService:
    def __init__(self, client):
        self._client = client
        self._pt_measurement_callback = None

    async def enable_pt_measurement_notifications(self):
        await self._client.start_notify(PressTemp_measurement_characteristic_id,
                                        self._pt_measurement_notification_handler)

    async def disable_pt_measurement_notifications(self):
        await self._client.stop_notify(PressTemp_measurement_characteristic_id)

    def set_pt_measurement_handler(self, callback):
        self._pt_measurement_callback = callback

    def _pt_measurement_notification_handler(self, sender, data):  # pylint: disable=unused-argument
        if self._pt_measurement_callback is not None:
            self._pt_measurement_callback(_parse_pt_measurement(data))
                
    async def get_pt(self):

        return PressTempMeasurement
