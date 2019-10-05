import React from "react"
import PropTypes from 'prop-types';

export {SearchBar}


class SearchBar extends React.Component {
    
    constructor(props) {
      super(props);
     
    }
        
    render() {
      return <div>
              <input name = "txtFilterText"
                     type = "text"
                     value = {this.props.filterText}
                     onChange={this.props.handleFilterTextChange} />
              <br/>
              <input name = "chbStockOnly"
                     type = "checkbox"
                     checked = {this.props.inStockOnly}
                     onChange={this.props.handleInStockOnlyChange} />
             </div>
    }
}

PropTypes.propTypes = {   
  name: PropTypes.string,
  price: PropTypes.string,
};
