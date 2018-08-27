fs => {

  fs.File = class File {
    async open(path) {
      this._fd = await fs.open(path);
    }

    async read(buffer) {
      await fs.read(buffer);
    }

    async eof() {
      return fs.eof(this._fd);
    }

    async close() {
      await fs.close(this._fd);
      this._fd = undefined;
    }
  };

  fs.readFile = (path, buf_size = 2048) => {
    return new Promise(async resolve => {
      const file = new fs.File();
      await file.open(path);

      const bufs = [];

      while (await !file.eof()) {
        const buf = new Buffer(buf_size);
        await file.read(buf);
        bufs.push(buf);
      }

      return Buffer.concat(bufs);
    });
  };

};