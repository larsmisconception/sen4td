from collections import namedtuple
import re

Dildo_measurement_characteristic_id = '6E400003-B5A3-F393-E0A9-E50E24DCCA9E'
vibrate_control_point_id = '6E400002-B5A3-F393-E0A9-E50E24DCCA9E'

DildoMeasurement = namedtuple('DildoMeasurement', ['dfP', 'dfT', 'dfX', 'dfY', 'dfZ'])


def _parse_pt_measurement(data):
    stTemp = data.decode("utf-8")
    st = re.split(";|:", stTemp)
    dfP=float(st[1])
    dfT=float(st[3])
    dfX=float(st[5])
    dfY=float(st[7])
    dfZ=float(st[9])
    return DildoMeasurement(dfP=dfP,
                                dfT=dfT,
                                dfX=dfX,
                                dfY=dfY,
                                dfZ=dfZ)


class DildoService:
    def __init__(self, client):
        self._client = client
        self._pt_measurement_callback = None

    async def enable_pt_measurement_notifications(self):
        await self._client.start_notify(Dildo_measurement_characteristic_id,
                                        self._pt_measurement_notification_handler)

    async def disable_pt_measurement_notifications(self):
        await self._client.stop_notify(Dildo_measurement_characteristic_id)

    def set_pt_measurement_handler(self, callback):
        self._pt_measurement_callback = callback

    def _pt_measurement_notification_handler(self, sender, data):  # pylint: disable=unused-argument
        if self._pt_measurement_callback is not None:
            self._pt_measurement_callback(_parse_pt_measurement(data))
                
    async def set_vibrate(self, vibrate: int):
        """sets the shock rate of the rizer to 1-255"""
        if vibrate < 0 or vibrate > 255:
            raise ValueError(
                "Invalid vibrate: choose 0-255"
            )
        stOut = str(vibrate).encode('utf-8')

        await self._client.write_gatt_char(vibrate_control_point_id, stOut)

