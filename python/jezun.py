import struct

shock_control_point_id = "00000003-0000-1000-8000-00805f9b34fb"

class Jezun:
    def __init__(self, client):
        self._client = client
        self._latest_challenge = None

 
    async def set_shock(self, rate: int):
        """sets the shock rate of the rizer to 1-19"""
        if rate < 0 or rate > 19:
            raise ValueError(
                "Invalid rate: choose 0-19"
            )
        byte_array = b"\xfd\x8b\xdb\xad\x95\x3c\x30" + rate.to_bytes(1, "little", signed=False) + b"\x9d"
        await self._client.write_gatt_char(shock_control_point_id, byte_array)
