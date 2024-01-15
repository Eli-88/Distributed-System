from dataclasses import dataclass
from typing import List, Iterator
import time
import hashlib


@dataclass
class Block:
    index: int
    prev_hash: bytes
    timestamp: int
    data: bytes
    nounce: int
    hash: bytes
    transaction_id: bytes


class BlockChain:
    def __init__(self) -> None:
        self.storage: List[Block] = []

    def __repr__(self) -> str:
        return f"{self.storage}"

    def append(self, block: Block):
        self.storage.append(block)

    def back(self) -> Block:
        return self.storage[-1]

    def __len__(self) -> int:
        return len(self.storage)

    def __iter__(self) -> Iterator[Block]:
        return iter(self.storage)

    def __getitem__(self, index: int) -> Block:
        return self.storage[index]


def create_genesis_block() -> Block:
    hash: bytes = hashlib.sha256(b"genesis").digest()
    return Block(
        index=0,
        prev_hash=hash,
        hash=hash,
        timestamp=time.time_ns(),
        data=b"",
        nounce=0,
        transaction_id=hash,
    )
