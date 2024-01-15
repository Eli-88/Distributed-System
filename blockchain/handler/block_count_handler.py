from aiohttp.web_request import Request
from aiohttp.web_response import StreamResponse
from handler.base_handler import BaseHandler
from block import BlockChain
from aiohttp.web import json_response
import logging


class BlockCountHandler(BaseHandler):
    def __init__(self, blockchain: BlockChain) -> None:
        self.blockchain = blockchain

    async def on_request(self, request: Request) -> StreamResponse:
        try:
            total_count: int = len(self.blockchain)
            return json_response({"count": total_count})
        except Exception as e:
            logging.exception(e)
            return json_response({"result": "invalid"})
