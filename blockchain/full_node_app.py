from transaction_pool import TransactionPool
from block import BlockChain
from typing import List
import asyncio
from task import initial_task, mining_task
from util import Addr
import sys
import logging
from handler.mine_result_handler import MineResultHandler
from handler.block_count_handler import BlockCountHandler
from handler.get_block_handler import GetBlockHandler
from handler.check_blockchain_handler import CheckBlockchainHandler
from server import Server


class FullNode:
    def __init__(
        self,
        local_addr: Addr,
        seeder_addr: Addr,
        peer_list: List[Addr],
        blockchain: BlockChain,
        transaction_pool: TransactionPool,
        server: Server,
    ) -> None:
        self.local_addr: Addr = local_addr
        self.seeder_addr: Addr = seeder_addr
        self.peer_list: List[Addr] = peer_list
        self.blockchain: BlockChain = blockchain
        self.transaction_pool = transaction_pool
        self.server: Server = server

        # handlers
        self.mine_result_handler = MineResultHandler(blockchain=blockchain)
        self.block_count_handler = BlockCountHandler(blockchain=blockchain)
        self.get_block_handler = GetBlockHandler(blockchain=blockchain)
        self.check_blockchain_handler = CheckBlockchainHandler(blockchain=blockchain)

        self.server.add_post("/mine_result", self.mine_result_handler.on_request)
        self.server.add_post("/total_block_count", self.block_count_handler.on_request)
        self.server.add_post("/get_block", self.get_block_handler.on_request)
        self.server.add_post(
            "/check_blockchain", self.check_blockchain_handler.on_request
        )

        self.mining_task = mining_task.MiningTask(
            blockchain=blockchain,
            transaction_pool=transaction_pool,
            peer_list=peer_list,
        )

    @classmethod
    async def create(cls, local_addr: Addr, seeder_addr: Addr) -> "FullNode":
        seeder_update_addr_url: str = (
            f"http://{seeder_addr.host}:{seeder_addr.port}/update_addr"
        )

        await initial_task.update_seeder(
            local_addr=local_addr, seeder_url=seeder_update_addr_url
        )

        peer_list: List[Addr] = await initial_task.get_peer_list(
            local_addr=local_addr, seeder_addr=seeder_addr
        )

        blockchain: BlockChain = await initial_task.initialize_blockchain(
            peer_list=peer_list
        )

        server = Server(localhost=local_addr.host, port=local_addr.port)

        return cls(
            local_addr=local_addr,
            seeder_addr=seeder_addr,
            peer_list=peer_list,
            blockchain=blockchain,
            transaction_pool=TransactionPool(),
            server=server,
        )

    async def run(self):
        await asyncio.gather(
            self.mining_task.start_mining(),
            self.server.run(),
            self.periodic_peer_list_update(),
        )

    async def periodic_peer_list_update(self):
        while True:
            self.peer_list = await initial_task.get_peer_list(
                local_addr=self.local_addr, seeder_addr=self.seeder_addr
            )

            logging.debug(f"updated peer list: {self.peer_list}")
            await asyncio.sleep(3)


async def main():
    if len(sys.argv) < 2:
        print("need a arguement for port")

    port: int = int(sys.argv[1])

    log_file_path = f"full_node_{port}.log"
    logging.basicConfig(
        filename=log_file_path,
        level=logging.DEBUG,
        format="%(asctime)s - %(levelname)s - %(message)s",
    )

    local_addr = Addr(host="localhost", port=port)
    seeder_addr = Addr(host="localhost", port=5555)

    full_node: FullNode = await FullNode.create(
        local_addr=local_addr, seeder_addr=seeder_addr
    )
    await asyncio.gather(full_node.run())


if __name__ == "__main__":
    asyncio.run(main())
