import mining
from transaction_pool import TransactionPool, Transaction
from block import BlockChain, Block
from util import Addr, post_msg
from typing import List, Dict, Any, Awaitable, Optional
import asyncio
import logging
import base64


class MiningTask:
    def __init__(
        self,
        blockchain: BlockChain,
        transaction_pool: TransactionPool,
        peer_list: List[Addr],
    ) -> None:
        self.blockchain: BlockChain = blockchain
        self.transaction_pool: TransactionPool = transaction_pool
        self.peer_list: List[Addr] = peer_list

    async def start_mining(self):
        logging.debug("start mining...")
        while True:
            transaction: Optional[Transaction] = self.transaction_pool.try_pop()
            if not transaction:
                transaction = Transaction.create(b"")

            assert transaction

            block: Block = await mining.mine_block(
                lastest_block=self.blockchain.back(), transaction=transaction
            )

            logging.debug(f"new block mined: {block}")

            request: Dict[str, Any] = dict()
            request["prev_hash"] = base64.b64encode(block.prev_hash).decode()
            request["timestamp"] = block.timestamp
            request["data"] = block.data.decode()
            request["nounce"] = block.nounce
            request["hash"] = base64.b64encode(block.hash).decode()
            request["transaction_id"] = base64.b64encode(block.transaction_id).decode()
            request["index"] = block.index

            votes_acquired = await self.broadcast_block(request)

            logging.debug(f"number of votes acquired: {votes_acquired}")

            minimum_votes: int = len(self.peer_list) // 3 + 1

            if votes_acquired >= minimum_votes:
                self.blockchain.append(block=block)
            else:
                if not transaction.is_blank:
                    self.transaction_pool.add(transaction=transaction)

    async def broadcast_block(self, request: Dict[str, Any]) -> int:
        url_list: List[str] = [
            f"http://{peer.host}:{peer.port}/mine_result" for peer in self.peer_list
        ]

        url_batches: List[List[str]] = [
            url_list[i : i + 10] for i in range(0, len(url_list), 10)
        ]

        batch_async_tasks: List[List[Awaitable[Dict[str, Any]]]] = []
        for url_batch in url_batches:
            batch_async_tasks.append(
                [post_msg(url=url, msg=request) for url in url_batch]
            )

        votes_acquired: int = 0

        for task in batch_async_tasks:
            responses = await asyncio.gather(*task, return_exceptions=True)
            for response in responses:
                if not isinstance(response, BaseException):
                    if response["result"] == "valid":
                        votes_acquired += 1

        return votes_acquired
