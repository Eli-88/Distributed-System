from aiohttp import web
from aiohttp.typedefs import Handler as WebHandler


class Server:
    def __init__(self, localhost: str, port: int) -> None:
        self.app = web.Application()
        self.host = localhost
        self.port = port

    async def run(self):
        runner = web.AppRunner(self.app)
        await runner.setup()

        site = web.TCPSite(runner, self.host, self.port)
        return await site.start()

    def add_get(self, path: str, handler: WebHandler):
        self.app.router.add_get(path=path, handler=handler)

    def add_post(self, path: str, handler: WebHandler):
        self.app.router.add_post(path=path, handler=handler)
