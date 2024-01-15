from aiohttp.web_request import Request
from aiohttp.web_response import StreamResponse
from handler.base_handler import BaseHandler
from block import BlockChain
from aiohttp.web import json_response
import logging


class CheckBlockchainHandler(BaseHandler):
    def __init__(self, blockchain: BlockChain) -> None:
        self.blockchain: BlockChain = blockchain

    async def on_request(self, request: Request) -> StreamResponse:
        try:
            return json_response({"chain": [str(b) for b in self.blockchain]})
        except Exception as e:
            logging.exception(e)

        return json_response({"result": "invalid"})
