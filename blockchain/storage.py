from abc import ABC, abstractmethod
from typing import Dict


class KVDataBase(ABC):
    @abstractmethod
    def create(self, key: bytes, value: bytes):
        pass

    @abstractmethod
    def delete(self, key: bytes):
        pass

    @abstractmethod
    def update(self, key: bytes, value: bytes):
        pass


class InMemoryKVDataBase(KVDataBase):
    def __init__(self) -> None:
        self.storage: Dict[bytes, bytes] = dict()

    def create(self, key: bytes, value: bytes):
        self.storage[key] = value

    def delete(self, key: bytes):
        del self.storage[key]

    def update(self, key: bytes, value: bytes):
        self.storage[key] = value
