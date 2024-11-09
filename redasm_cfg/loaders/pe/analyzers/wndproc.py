def wndproc_isenabled(pe):
    c = pe.classifier
    return not (c.is_borland or c.is_dotnet or c.is_visualbasic)


def wndproc_execute(pe):
    pass
