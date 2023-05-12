import lib.pydatahub as pydatahub
import lib.pyterminus as pyterminus

simple_sealed_file = pydatahub.SimpleSealedFile("123", False)
print(type(simple_sealed_file))

var1 = pydatahub.CryptoBaseType(1)
print(var1)

crypto_base = pydatahub.CryptoBase(var1)
print(crypto_base)

var2 = pyterminus.CryptoPackType(1)
print(var2)

crypto_pack = pyterminus.CryptoPack(var2)

private_key = crypto_pack.gen_ecc_private_key()
public_key = crypto_pack.gen_ecc_public_key_from_private_key(private_key)
hash_256 = crypto_pack.hash_256(private_key)
print(type(hash_256))
test = crypto_base.hash_256(private_key, public_key)
print(test)

encrypt_message_with_prefix = crypto_base.encrypt_message_with_prefix(public_key, hash_256, 111, private_key)
print(encrypt_message_with_prefix)

singleDataOnchainResult = pyterminus.SingleDataOnchainResult(crypto_pack)
print(singleDataOnchainResult)

array = [private_key, public_key]

tool = pydatahub.Tool(crypto_base)
array2 = [1, 2, 3]
tool.test(array2)


print(tool)
tool.write_batch(simple_sealed_file, array, public_key)

#tool.seal_file("/home/zhangsz/test.txt", "/home/zhangsz/test.txt", "/home/zhangsz/test.txt", public_key, hash_256)
