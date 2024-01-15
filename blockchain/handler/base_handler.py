from abc import ABC, abstractmethod
from aiohttp.web_request import Request
from aiohttp.web_response import StreamResponse


class BaseHandler(ABC):
    @abstractmethod
    async def on_request(self, request: Request) -> StreamResponse:
        pass
