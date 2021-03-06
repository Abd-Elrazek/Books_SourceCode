"""
Fibonacci generator implemented "by hand" without generator objects
"""

# BEGIN FIBO_BY_HAND
class Fibonacci:

    def __iter__(self):
        return FibonacciGenerator()


class FibonacciGenerator:

    def __init__(self):
        self.a = 0
        self.b = 1

    def __next__(self):
        result = self.a
        self.a, self.b = self.b, self.a + self.b
        return result

    def __iter__(self):
        return self
# END FIBO_BY_HAND

# for comparison, this is the usual implementation of a Fibonacci
# generator in Python:


def fibonacci():
    a, b = 0, 1
    while True:
        yield a
        a, b = b, a + b


if __name__ == '__main__':

    # 验证Fibonacci()生成器, 没有使用yield
    from itertools import islice
    print(list(islice(Fibonacci(), 15))) # [0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377]

    import types
    F = Fibonacci()
    F_gen = FibonacciGenerator()
    f_gen = fibonacci()
    print(isinstance(F, types.GeneratorType)) # False
    print(isinstance(F_gen, types.GeneratorType)) # False
    print(isinstance(f_gen, types.GeneratorType)) # True
    print(isinstance(fibonacci(), types.GeneratorType)) # 和f_gen相同, True
    print(isinstance(fibonacci, types.GeneratorType)) # 和普通函数完全相同, False
    from collections import abc
    print(isinstance(F, abc.Iterator)) # False
    print(isinstance(F_gen, abc.Iterator)) # True
    print(isinstance(f_gen, abc.Iterator)) # True
    print(isinstance(fibonacci(), abc.Iterator)) # 和f_gen相同, True
    print(isinstance(fibonacci, abc.Iterator)) # 和普通函数完全相同, False

    # 对比Fibonacci()生成器和fibonacci()生成器, 完全相同
    for x, y in zip(Fibonacci(), fibonacci()):
        assert x == y, '%s != %s' % (x, y)
        print(x)
        if x > 10**10:
            break
    print('etc...')
