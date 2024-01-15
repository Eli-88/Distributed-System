from block import Block
import consensus
import aiohttp
from typing import Dict, Any, List, Awaitable
import time
import asyncio
from transaction_pool import Transaction


async def mine_block(lastest_block: Block, transaction: Transaction) -> Block:
    prev_hash: bytes = lastest_block.hash
    index = lastest_block.index + 1

    data = b""
    if transaction:
        data = transaction.data

    return await consensus.proof_of_work(
        index=index,
        prev_hash=prev_hash,
        data=data,
        difficulty=b"000",
        timestamp=time.time_ns(),
        transaction_id=transaction.transaction_id,
    )


async def send_block(msg: Dict[str, Any], url: str):
    async with aiohttp.ClientSession() as session:
        await session.post(url=url, json=msg)


async def broadcast_mined_block_to_all(block: Block, url_batches: List[List[str]]):
    response: Dict[str, Any] = dict()
    response["index"] = block.index
    response["prev_hash"] = block.prev_hash.decode()
    response["timestamp"] = block.timestamp
    response["data"] = block.data.decode()
    response["nounce"] = block.nounce
    response["hash"] = block.hash.decode()
    response["transaction_id"] = block.transaction_id.decode()

    batch_async_tasks: List[List[Awaitable[Any]]] = []
    for batch in url_batches:
        batch_async_tasks.append([send_block(msg=response, url=url) for url in batch])

    for task in batch_async_tasks:
        await asyncio.gather(*task)
