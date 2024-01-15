from aiohttp.web_request import Request
from aiohttp.web_response import StreamResponse
from handler.base_handler import BaseHandler
from block import BlockChain, Block
from aiohttp.web import json_response
import base64
import logging


class GetBlockHandler(BaseHandler):
    def __init__(self, blockchain: BlockChain) -> None:
        self.blockchain: BlockChain = blockchain

    async def on_request(self, request: Request) -> StreamResponse:
        req = await request.json()
        index: int = int(req["index"])

        logging.debug(f"get block index: {index}")

        block: Block = self.blockchain[index]

        return json_response(
            {
                "index": block.index,
                "prev_hash": base64.b64encode(block.prev_hash).decode(),
                "timestamp": block.timestamp,
                "data": block.data.decode(),
                "nounce": block.nounce,
                "hash": base64.b64encode(block.hash).decode(),
                "transaction_id": base64.b64encode(block.transaction_id).decode(),
            }
        )
