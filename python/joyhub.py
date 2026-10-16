import struct

vibrate_control_point_id = "0000ffa1-0000-1000-8000-00805f9b34fb"

class Joyhub:
    def __init__(self, client):
        self._client = client
        self._latest_challenge = None

 
    async def set_vibrate(self, vibrate: int):
        """sets the shock rate of the rizer to 1-255"""
        if vibrate < 0 or vibrate > 255:
            raise ValueError(
                "Invalid vibrate: choose 0-255"
            )
        byte_array = b"\xa0\x03" + vibrate.to_bytes(1, "little", signed=False) + b"\x00\x00\x00\xaa"

        await self._client.write_gatt_char(vibrate_control_point_id, byte_array)
