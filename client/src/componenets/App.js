import '../styles/global.css'
import '../styles/App.css'
import { useState } from 'react'
import down_arrow from '../assets/down_arrow.png'

function App() {
  const [ slot_active, set_slot_active ] = useState({})
  const [ slots, set_slots ] = useState({
    '0': { watch_context: 'watch-context', backup_context: 'backup-context', deleted: false },
    '1': { watch_context: 'watch-context', backup_context: 'backup-context', deleted: false },
  })
  const [ crosshair, set_crosshair ] = useState(-1)
  // for new slots, the correct latest ids should be calculated
  const [ new_slot_id, set_new_slot_id ] = useState(Object.keys(slots).length)

  function toggle_slot(slot) { 
    set_slot_active({
      [slot]: !slot_active[slot]
    })
    set_crosshair(slot)
  }

  function modify_watch_context(target, key) {
    set_slots({
      ...slots,
      [key]: { ...slots[key], watch_context: target.value }
    })
  }

  function modify_backup_context(target, key) {
    set_slots({
      ...slots,
      [key]: { ...slots[key], backup_context: target.value }
    })
  }

  function remove_slot() {
    if ( crosshair > -1 ) {
      set_slots({
            ...slots,
            [crosshair]: { ...slots[crosshair], deleted: true }
          })
      set_crosshair(-1)
    }
    
  }

  function add_slot() {
    set_slots({
      ...slots,
      [new_slot_id]: { watch_context: 'new_slot', 
                      backup_context: 'new_slot', 
                      deleted: false }
    })
    set_new_slot_id(Object.keys(slots).length + 1)
  }

  return (
    <>
    <div className='AO' >


      { Object.keys(slots).map( key => (
        !slots[key].deleted && <div className={`slot ${slot_active[key] ? 'slot-active' : ''}`} onClick={ () => toggle_slot(key) }>
          <label style={{color: '#009900'}}>Slot {key}</label>
          <label>Watch this location for changes</label>
          <input value={slots[key].watch_context} onChange={e => modify_watch_context(e.target, key)} ></input> {/* directory to watch */}
          <img src={down_arrow} alt='down_arrow'></img>
          <label>Upon changes, create new copies in this location</label>
          <input value={slots[key].backup_context} onChange={e => modify_backup_context(e.target, key)} ></input> {/* backup directory */}
        </div>
      ) ) }

    </div>

    <div className='buttons'>
      <button style={{backgroundColor: '#66b032'}}>Save all</button>
      <button style={{backgroundColor: '#00bfff'}} onClick={add_slot} >Add slot</button>
      <button style={{backgroundColor: '#cc0000'}} onClick={remove_slot}>Remove slot</button>
    </div>

    </>
  )
}

export default App;
