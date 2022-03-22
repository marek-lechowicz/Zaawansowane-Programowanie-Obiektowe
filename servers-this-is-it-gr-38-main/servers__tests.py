import unittest
from collections import Counter


from servers import ListServer, Product, Client, MapServer, TooManyProductsFoundError

server_types = (ListServer, MapServer)


class ServerTest(unittest.TestCase):

    def test_get_entries_returns_proper_entries(self):
        products = [Product('P12', 1), Product('PP234', 2), Product('PP235', 1)]
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries(2)

            self.assertEqual(Counter([products[2], products[1]]), Counter(entries))

    def test_get_entries_too_many_products_found(self):
        products = [Product('PA12', 2), Product('PD12', 3), Product('PC13', 4), Product('PL22', 1),
                    Product('PM55', 8), Product('P11', 1)]
        for server_type in server_types:
            server = server_type(products)
            with self.assertRaises(TooManyProductsFoundError):
                server.get_entries(2)

    def test_get_entries(self):
        products = [Product('PP233', 15), Product('PP234', 10), Product('PP235', 1)]
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries(2)
            self.assertEqual([products[2], products[1], products[0]], entries)

        products = [Product('P233', 15), Product('P234', 10), Product('P235', 1)]
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries(1)
            self.assertEqual([products[2], products[1], products[0]], entries)



class ClientTest(unittest.TestCase):
    def test_total_price_for_normal_execution(self):
        products = [Product('PP234', 2), Product('PP235', 3)]
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(5, client.get_total_price(2))


    def test_total_price_for_error_execution(self):
        products = [Product('PP234', 2), Product('PA235', 3), Product('AB236', 2), Product('PA23', 5)]
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(None, client.get_total_price(2))

    def test_total_price_for_zero_entries(self):
        products = [Product('P234', 2), Product('P235', 3), Product('A236', 2), Product('P23', 5)]
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(None, client.get_total_price(2))


class ProductTest(unittest.TestCase):
    def test___init__(self):
        products = [Product('PP345', 2), Product('P34', 3), Product('pr1', 1)]

        with self.assertRaises(ValueError):
            Product('', 5)

        with self.assertRaises(ValueError):
            Product('p', 5)

        with self.assertRaises(ValueError):
            Product('5', 5)

        self.assertEqual('PP345', products[0].name)
        self.assertEqual('P34', products[1].name)
        self.assertEqual('pr1', products[2].name)


if __name__ == '__main__':
    unittest.main()

# TODO napisać testy