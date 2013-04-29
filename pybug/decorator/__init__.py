from decorator import decorator


@decorator
def constfunction(f, *args, **kwargs):
    """
    Decorator to maintain constness of parameters for a function.

    This should be used on any function that takes ndarrays as parameters.
    This helps protect against potential subtle bugs caused by the pass by
    reference nature of python arguments. Copy
    is assumed to a shallow copy.

    :param f: function to wrap
    :param args: function f's args
    :param kwargs: function f's kwargs
                   copy {True, False}
                       If False then don't copy the parameters - use with
                       caution
    """
    copy = kwargs.get('copy', True)
    if copy:
        args, kwargs = _const_helper(args, kwargs)
    return f(*args, **kwargs)


@decorator
def constmethod(method, *args, **kwargs):
    """
    Decorator to maintain constness of parameters for a class method.

    This should be used on any method that takes ndarrays as parameters.
    This helps protect against potential subtle bugs caused by the pass by
    reference nature of python arguments. Copy is assumed to a shallow copy.

    :param method: class method to wrap
    :param args: method's args
    :param kwargs: method's kwargs
                   copy {True, False}
                       If False then don't copy the parameters - use with
                       caution
    """
    copy = kwargs.get('copy', True)
    if copy:
        self = args[0]
        print 'self is: ' + str(self)
        args, kwargs = _const_helper(args[1:], kwargs)
        args = self + args
    return method(*args, **kwargs)


def _const_helper(*args, **kwargs):
    """
    Returns a copy of the args and kwargs passed in.
    """
    print 'args are: ' + str(args)
    print 'kwargs are: ' + str(kwargs)
    args = [x.copy() for x in args]
    kwargs = dict((k, v.copy()) for k, v in kwargs.items())
    return args, kwargs
