from aiohttp import web
from aiohttp.web_request import Request
from aiohttp.web_response import StreamResponse
from aiohttp.web import json_response
from util import Addr
from typing import Set, Dict, Any
import logging
import asyncio


peer_list: Set[Addr] = set()
routes = web.RouteTableDef()


@routes.post("/update_addr")
async def on_peer_update(request: Request) -> StreamResponse:
    data = await request.json()
    host: str = data["host"]
    port: int = int(data["port"])
    peer_list.add(Addr(host=host, port=port))
    return json_response({"result": "success"})


@routes.post("/get_all_peer")
async def on_get_all_peer(request: Request) -> StreamResponse:
    data = await request.json()
    remote_host: str = data["host"]
    remote_port: int = int(data["port"])

    logging.info(f"get all peer request from {remote_host}:{remote_port}")

    remote_addr: Addr = Addr(host=remote_host, port=remote_port)

    response: Dict[str, Any] = dict()
    response["peers"] = [
        f"{addr.host}:{addr.port}" for addr in peer_list if addr != remote_addr
    ]

    return json_response(response)


async def start_server():
    app = web.Application()
    app.add_routes(routes)

    runner = web.AppRunner(app)
    await runner.setup()

    site = web.TCPSite(runner, "localhost", 5555)
    await site.start()

    print("Server started on http://localhost:5555")


if __name__ == "__main__":
    loop = asyncio.new_event_loop()
    loop.create_task(start_server())
    loop.run_forever()
