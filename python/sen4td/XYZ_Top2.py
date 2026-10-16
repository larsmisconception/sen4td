from collections import namedtuple
import re

PressTemp_measurement_characteristic_id = '6E400003-B5A3-F393-E0A9-E50E24DCCA99'

PressTempMeasurement = namedtuple('PressTempMeasurement', ['dfT1', 'dfX1', 'dfY1', 'dfZ1', 'dfAX1', 'dfAY1', 'dfAZ1','dfT2', 'dfX2', 'dfY2', 'dfZ2', 'dfAX2', 'dfAY2', 'dfAZ2'])


def _parse_pt_measurement(data):
    stTemp = data.decode("utf-8")
    st = re.split(";|:", stTemp)
    dfT1=float(st[1])
    dfX1=float(st[3])
    dfY1=float(st[5])
    dfZ1=float(st[7])
    dfAX1=float(st[9])
    dfAY1=float(st[11])
    dfAZ1=float(st[13])
    dfT2=float(st[15])
    dfX2=float(st[17])
    dfY2=float(st[19])
    dfZ2=float(st[21])
    dfAX2=float(st[23])
    dfAY2=float(st[25])
    dfAZ2=float(st[27])
    return PressTempMeasurement(dfT1=dfT1,
                                dfX1=dfX1,
                                dfY1=dfY1,
                                dfZ1=dfZ1,
                                dfAX1=dfAX1,
                                dfAY1=dfAY1,
                                dfAZ1=dfAZ1,
                                dfT2=dfT2,
                                dfX2=dfX2,
                                dfY2=dfY2,
                                dfZ2=dfZ2,
                                dfAX2=dfAX2,
                                dfAY2=dfAY2,
                                dfAZ2=dfAZ2)


class XYZ_TopService2:
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
