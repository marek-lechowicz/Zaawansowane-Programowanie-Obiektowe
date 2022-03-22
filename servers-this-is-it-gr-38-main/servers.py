#!/usr/bin/python
# -*- coding: utf-8 -*-
 
from typing import Optional, TypeVar, List
from abc import ABC, abstractmethod
import re

class Product:
    # FIXME: klasa powinna posiadać metodę inicjalizacyjną przyjmującą argumenty wyrażające nazwę produktu (typu str) i jego cenę (typu float) -- w takiej kolejności -- i ustawiającą atrybuty `name` (typu str) oraz `price` (typu float)

    def __init__(self, name: str, price: float):
        if not re.fullmatch('^[a-zA-Z]+\d+$', name):
            raise ValueError
        self.name = name
        self.price = price

    def __eq__(self, other):
        return (self.name == other.name) and (self.price == other.price)  # FIXME: zwróć odpowiednią wartość
 
    def __hash__(self):
        return hash((self.name, self.price))


class ServerError(Exception):
    """ Ogólny błąd dotyczący serwera """

    def __init__(self, server, msg=None):
        if msg is None:
            msg = f"An error occured with server: {server}"
        super().__init__(msg)
        self.server = server


class TooManyProductsFoundError(ServerError):
    # Reprezentuje wyjątek związany ze znalezieniem zbyt dużej liczby produktów.
    """ Znaleziono za dużo produktów """

    def __init__(self, server, number_of_items: int):
        super().__init__(server, msg=f"Server Error: Too many product found in server {server}! "
                                     f"Expected 3, got: {number_of_items}")
        self.number_of_items = number_of_items

# FIXME: Każada z poniższych klas serwerów powinna posiadać:
#   (1) metodę inicjalizacyjną przyjmującą listę obiektów typu `Product` i ustawiającą atrybut `products` zgodnie z typem reprezentacji produktów na danym serwerze,
#   (2) możliwość odwołania się do atrybutu klasowego `n_max_returned_entries` (typu int) wyrażający maksymalną dopuszczalną liczbę wyników wyszukiwania,
#   (3) możliwość odwołania się do metody `get_entries(self, n_letters)` zwracającą listę produktów spełniających kryterium wyszukiwania


class Server(ABC):

    n_max_returned_entries = 3

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    @abstractmethod
    def download_list(self, n_letters: int):
        pass

    def get_entries(self, n_letters: int) -> Optional[List[Product]]:
        search_results = list()
        product_list: List[Product] = self.download_list(n_letters)

        for product in product_list:
            if re.fullmatch('^[a-zA-Z]{{{n}}}\\d{{2,3}}$'.format(n=n_letters), product.name):
                search_results.append(product)

        if search_results is not None:
            if len(search_results) > self.n_max_returned_entries:
                raise TooManyProductsFoundError(self, len(search_results))

        return sorted(search_results, key=lambda product_from_list: product_from_list.price)


class ListServer(Server):

    def __init__(self, products: List[Product], *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.products = products

    def download_list(self, n_letters: int) -> List[Product]:
        return self.products


class MapServer(Server):

    def __init__(self, products: List[Product], *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.products = {el.name: el for el in products}

    def download_list(self, n_letters: int) -> List[Product]:
        return list(self.products.values())


ServerType = TypeVar('ServerType', bound=Server)

class Client:
    # FIXME: klasa powinna posiadać metodę inicjalizacyjną przyjmującą obiekt reprezentujący serwer
    def __init__(self, server: ServerType):
        self.server = server

# TODO przetestować wyjątki
    def get_total_price(self, n_letters: Optional[int]) -> Optional[float]:
        total_price: float = 0.0
        try:
            list_of_products: List[Product] = self.server.get_entries(n_letters)

        except ServerError as error:
            print("An error has occurred.\nDescription:\n", error)
            return None
        if not list_of_products:
            return None
        for product in list_of_products:
            total_price += product.price
        return total_price


