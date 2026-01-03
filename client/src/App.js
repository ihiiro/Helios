import './App.css';
import down_arrow from './assets/down_arrow.png'

function App() {
  return (
    <div className='AO' >
      {/* the default slot which is visible always */}
      <div className='slot'>
        <label>Watch this location for changes</label>
        <input type='file' webkitdirectory ></input> {/* directory to watch */}
        <img src={down_arrow} alt='down_arrow'></img>
        <label>Upon changes, create new copies in this location</label>
        <input></input> {/* backup directory */}
      </div>
    </div>
  )
}

export default App;
