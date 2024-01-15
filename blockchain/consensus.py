import hashlib
from block import Block
import asyncio


def calculate_hash(
    prev_hash: bytes, data: bytes, nounce: int, timestamp: int, transaction_id: bytes
):
    value = f"{prev_hash}{data}{nounce}{transaction_id}".encode()
    return hashlib.sha256(value).digest()


async def proof_of_work(
    index: int,
    prev_hash: bytes,
    data: bytes,
    difficulty: bytes,
    timestamp: int,
    transaction_id: bytes,
) -> Block:
    nounce = 0

    result = b""
    time_slice_counter = 0

    while not result.startswith(difficulty):
        result = calculate_hash(
            prev_hash=prev_hash,
            data=data,
            nounce=nounce,
            timestamp=timestamp,
            transaction_id=transaction_id,
        )
        nounce += 1

        if time_slice_counter > 10000:
            time_slice_counter = 0
            await asyncio.sleep(0)

        time_slice_counter += 1

    return Block(
        index=index,
        prev_hash=prev_hash,
        timestamp=timestamp,
        data=data,
        nounce=nounce,
        hash=result,
        transaction_id=transaction_id,
    )
