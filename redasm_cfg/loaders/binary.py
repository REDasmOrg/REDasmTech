import redasm


def init():
    redasm.memory.map(0, len(redasm.file))
    redasm.memory.copy(0, 0, len(redasm.file))
    return True


redasm.register_loader("binary", "Binary", init)
