import struct

vibrate_control_point_id = "45440002-0023-4bd4-bbd5-a6920e4c5653"

class Gush:
    def __init__(self, client):
        self._client = client
        self._latest_challenge = None

 
    async def set_vibrate(self, vibrate: int):
        """sets the shock rate of the rizer to 1-20"""
        if vibrate < 0 or vibrate > 20:
            raise ValueError(
                "Invalid vibrate: choose 0-20"
            )
        
        byte_array = b"Vibrate:" + str(vibrate).encode('utf-8')
        await self._client.write_gatt_char(vibrate_control_point_id, byte_array)
