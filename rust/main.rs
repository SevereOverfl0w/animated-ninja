extern crate http;
extern crate serialize;

use http::client::RequestWriter;
use http::method::Get;
use std::str;
use serialize::json;

struct Currency {
    currency_code: ~str,
    _15m: f64,
    last: f64,
    buy: f64,
    sell: f64,
    symbol: ~str
}

fn main(){
    let mut currencies: Vec<Currency> = Vec::new();

    let request: RequestWriter = RequestWriter::new(Get, from_str("http://blockchain.info/ticker").expect("Invalid Url :("))
                                                         .unwrap();

    let mut response = match request.read_response(){
        Ok(response) => response,
        Err(_response) => fail!("Failed to get response."),
    };

    let body = match response.read_to_end() {
        Ok(body) => body,
        Err(err) => fail!("Read_to_end phail: {}", err),
    };

    let currencies_json = match json::from_str(str::from_utf8(body.as_slice()).expect("Uh oh, response wasn't UTF-8")) {
        Ok(json) => json,
        Err(err) => fail!("Failed to decode JSON string: {}", err)
    };

    let currencies_object = match currencies_json.as_object(){
        Some(v) => v,
        _ => fail!("Unable to convert currencies_json to object")
    };

    for (currency, currency_data) in currencies_object.iter(){
//        let currency_tmp = Currency {
//            currency_code: currency.clone(),
//        };
        let currency_data_object = match currency_data.as_object(){
          Some(v) => v,
          _ => fail!("Unable to convert {}'s data to object", currency)
        };

        currencies.push(Currency{
            currency_code: currency,
            _15m: currency_data_object.find(&"15m".to_owned()).unwrap(),
            last: currency_data_object.find(&"last".to_owned()).unwrap(),
            buy: currency_data_object.find(&"buy".to_owned()).unwrap(),
            sell: currency_data_object.find(&"sell".to_owned()).unwrap(),
            symbol: currency_data_object.find(&"symbol".to_owned()).unwrap()
        });
    }
}
