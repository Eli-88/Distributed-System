from typing import Tuple, Any, Callable, Type, Awaitable, NamedTuple, Dict
import logging
import aiohttp


class RetryException(Exception):
    def __init__(self, msg: str) -> None:
        super().__init__(msg)


def async_retry(max_tries: int, exception_types: Tuple[Type[Exception]]):
    def decorator(func: Callable[..., Awaitable[Any]]):
        async def wrapper(*args: Any, **kwargs: Any):
            retries = 0
            while retries < max_tries:
                try:
                    return await func(*args, **kwargs)
                except exception_types as e:
                    logging.error(e)
                    retries += 1
            raise RetryException(f"fail after {max_tries} retries")

        return wrapper

    return decorator


class Addr(NamedTuple):
    host: str
    port: int


async def post_msg(msg: Dict[str, Any], url: str) -> Dict[str, Any]:
    async with aiohttp.ClientSession() as session:
        response = await session.post(url=url, json=msg)
        return await response.json()
