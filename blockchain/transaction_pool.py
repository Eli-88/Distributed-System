from typing import List, Optional, Iterable
import time
import hashlib


class Transaction:
    @classmethod
    def create(cls, data: bytes) -> "Transaction":
        value = f"{data}{time.time_ns()}".encode()
        transaction_id: bytes = hashlib.sha256(value).digest()
        return cls(data=data, transaction_id=transaction_id)

    def __init__(
        self, data: bytes, transaction_id: bytes, is_blank: bool = False
    ) -> None:
        self.data = data
        self.transaction_id = transaction_id
        self.is_blank = is_blank


class TransactionPool:
    def __init__(self) -> None:
        self.pending_storage: List[Transaction] = []

    def __iter__(self) -> Iterable[Transaction]:
        return iter(self.pending_storage)

    def add(self, transaction: Transaction):
        self.pending_storage.append(transaction)

    def try_pop(self) -> Optional[Transaction]:
        if len(self.pending_storage) != 0:
            return self.pending_storage.pop()
        return None
