from aiohttp.web_request import Request
from aiohttp.web_response import StreamResponse
from aiohttp.web import json_response
import base64
from block import Block, BlockChain
import logging
from handler.base_handler import BaseHandler


class MineResultHandler(BaseHandler):
    def __init__(self, blockchain: BlockChain) -> None:
        self.blockchain = blockchain

    async def on_request(self, request: Request) -> StreamResponse:
        try:
            req = await request.json()
            logging.debug(f"mine result validation request: {req}")
            prev_hash: bytes = base64.b64decode(req["prev_hash"].encode())
            timestamp: int = int(req["timestamp"])
            data: bytes = req["data"].encode()
            nounce: int = int(req["nounce"])
            hash: bytes = base64.b64decode(req["hash"].encode())
            transaction_id: bytes = base64.b64decode(req["transaction_id"].encode())
            index: int = int(req["index"])

            current_block = self.blockchain.back()

            logging.debug(f"current block in chain: {current_block}")
            if (
                current_block.index + 1 == index
                and current_block.hash == prev_hash
                and hash.startswith(b"000")
            ):
                logging.debug("block validation approved")
                self.blockchain.append(
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
                return json_response({"result": "valid"})
        except Exception as e:
            logging.exception(e)

        logging.debug("block validation fails")
        return json_response({"result": "invalid"})
