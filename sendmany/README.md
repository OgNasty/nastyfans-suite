# sendmany

## WARNING

This code is for educational purposes only. Use at your own risk.

## About

This tool emulates the `bitcoin-cli sendmany` interface for input. For output
it prints shell commands to generate the raw transaction.

## Usage Example

Create a raw transaction that does:

- send 1.3 BTC to mh4J1GtZfjd4qZRPwPorLs5stNaKXGKBxr
- send 3.1 BTC to mx5bDqrtjkxEPHEUopyYKLqAhmuHEnbmLq
- include 0.0005 BTC transaction fee
- send change to mmGbssaHHGpLDrwV3HawNvLj91wfQg5Aip
- use funds in "testaccount"

```
export CHANGE_ADDRESS=mmGbssaHHGpLDrwV3HawNvLj91wfQg5Aip
export TX_FEE=0.0005
export LISTUNSPENT=/var/lib/nastyfans/listunspent
export ACCOUNT_ROOT=/var/lib/nastyfans/accounts
./sendmany testaccount '{
	"mh4J1GtZfjd4qZRPwPorLs5stNaKXGKBxr":1.3,
	"mmGbssaHHGpLDrwV3HawNvLj91wfQg5Aip":0,
	"mx5bDqrtjkxEPHEUopyYKLqAhmuHEnbmLq":3.1
	}'
```
