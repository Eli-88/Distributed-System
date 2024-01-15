from block import Block, BlockChain, create_genesis_block
from util import post_msg, Addr
from typing import Dict, Any, List, Awaitable
import asyncio
import random
import logging
import base64


async def update_seeder(local_addr: Addr, seeder_url: str):
    msg: Dict[str, Any] = dict()
    msg["host"] = local_addr.host
    msg["port"] = local_addr.port

    await post_msg(url=seeder_url, msg=msg)


async def get_peer_list(local_addr: Addr, seeder_addr: Addr) -> List[Addr]:
    msg: Dict[str, str | int] = {}
    msg["host"] = local_addr.host
    msg["port"] = local_addr.port

    seeder_url: str = f"http://{seeder_addr.host}:{seeder_addr.port}/get_all_peer"

    peer_list = await post_msg(url=seeder_url, msg=msg)

    if not peer_list:
        logging.error("no peer list received")
        return []

    peer_data: List[str] = peer_list["peers"]
    logging.debug(f"received peer list: {peer_list}")

    return [
        Addr(host=host, port=int(port))
        for host, port in (peer.split(":") for peer in peer_data)
    ]


async def initialize_blockchain(peer_list: List[Addr]) -> BlockChain:
    if len(peer_list) == 0:
        logging.debug("no peer list avaliable, generate genesis block")
        genesis_block: Block = create_genesis_block()
        chain = BlockChain()
        chain.append(genesis_block)
        return chain

    msg: Dict[str, Any] = dict()

    addr = peer_list[random.randint(0, len(peer_list) - 1)]
    block_count_url = f"http://{addr.host}:{addr.port}/total_block_count"

    block_count_data = await post_msg(url=block_count_url, msg=msg)
    if not block_count_data:
        raise RuntimeError("initialize_block_error for request block count")

    block_count: int = block_count_data["count"]

    block_url = f"http://{addr.host}:{addr.port}/get_block"
    all_block_requests: List[Dict[str, int]] = [
        {"index": idx} for idx in range(0, block_count)
    ]
    block_req_tasks: List[Awaitable[Any]] = [
        post_msg(msg=req, url=block_url) for req in all_block_requests
    ]
    block_req_task_batch: List[List[Awaitable[Any]]] = [
        block_req_tasks[i : i + 100] for i in range(0, block_count, 100)
    ]

    chain = BlockChain()
    for req_batch in block_req_task_batch:
        block_responses = await asyncio.gather(*req_batch)
        for block_response in block_responses:
            logging.debug(f"get block response: {block_response}")
            index: int = block_response["index"]
            prev_hash: bytes = base64.b64decode(block_response["prev_hash"].encode())
            timestamp: int = block_response["timestamp"]
            data: bytes = block_response["data"].encode()
            nounce: int = block_response["nounce"]
            hash: bytes = base64.b64decode(block_response["hash"].encode())
            transaction_id: bytes = base64.b64decode(
                block_response["transaction_id"].encode()
            )

            chain.append(
                Block(
                    index=index,
                    prev_hash=prev_hash,
                    timestamp=timestamp,
                    data=data,
                    nounce=nounce,
                    hash=hash,
                    transaction_id=transaction_id,
                )
            )
    return chain
